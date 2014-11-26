#include "stdafx.h"
#include "shadow.h"

GLuint depth_texture_id;
GLuint fboId;
GLhandleARB shadowShaderId;
GLuint shadowMapUniform;

void generateShadowFBO()
{
	int shadowMapWidth = SHADOW_MAP_WIDTH;
	int shadowMapHeight = SHADOW_MAP_HEIGHT;

	//GLfloat borderColor[4] = {0,0,0,0};

	GLenum FBOstatus;

	// Try to use a texture depth component
	glGenTextures(1, &depth_texture_id);
	glBindTexture(GL_TEXTURE_2D, depth_texture_id);

	// GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Remove artefact on the edges of the shadowmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );



	// No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// create a framebuffer object
	glGenFramebuffersEXT(1, &fboId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

	// Instruct openGL that we won't bind a color texture with the currently binded FBO
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// attach the texture to FBO depth attachment point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depth_texture_id, 0);

	// check FBO status
	FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");

	// switch back to window-system-provided framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


void setupMatrices(const flt eye[4], Vec3f center, bool lightSource)
{
	bool parallel = zero(eye[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	int width, height;
	if (lightSource) {
		width = SHADOW_MAP_WIDTH;
		height = SHADOW_MAP_HEIGHT;
	} else {
		extern int wWidth, wHeight;
		width = wWidth;
		height = wHeight;
	}
	int sz = lightSource ? 50 : 10;
	if(!parallel) gluPerspective(lightSource ? 100 : 45,  width / flt(height), 0.1, 40000);
	else glOrtho(-sz, sz, -sz, sz, 0.1, 40000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vec3f neye = Vec3f(eye[0], eye[1], eye[2]);
	if (parallel) neye = center + neye * 10;
	else neye = neye * (1 / eye[3]);
	gluLookAt(neye[0],neye[1],neye[2],center[0],center[1],center[2],0,1,0);
}

void setTextureMatrix(void)
{
	static double modelView[16];
	static double projection[16];

	// This is matrix transform every coordinate x,y,z
	// x = x* 0.5 + 0.5 
	// y = y* 0.5 + 0.5 
	// z = z* 0.5 + 0.5 
	// Moving from unit cube [-1,1] to [0,1]
	const GLdouble bias[16] = {
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0 };

	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);

	glLoadIdentity();
	glLoadMatrixd(bias);

	// concatating all matrice into one.
	glMultMatrixd(projection);
	glMultMatrixd(modelView);

	// Go back to normal matrix mode
	glMatrixMode(GL_MODELVIEW);
}

// During translation, we also have to maintain the GL_TEXTURE8, used in the shadow shader
// to determine if a vertex is in the shadow.
void startTranslate(float x, float y, float z)
{
	glPushMatrix();
	glTranslatef(x, y, z);

	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glPushMatrix();
	glTranslatef(x, y, z);
}

void endTranslate()
{
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void drawObjects(void)
{
	// Ground
	glColor4f(0.3f, 0.3f, 0.3f, 1);
	glBegin(GL_QUADS);
	glVertex3f(-35, 2, -35);
	glVertex3f(-35, 2, 15);
	glVertex3f(15, 2, 15);
	glVertex3f(15, 2, -35);
	glEnd();

	glColor4f(0.9f, 0.9f, 0.9f, 1);

	// Instead of calling glTranslatef, we need a custom function that also maintain the light matrix
	startTranslate(0, 4, -16);
	glutSolidCube(4);
	endTranslate();
	startTranslate(0, 8, -10);
	glutSolidCylinder(1, 1, 100, 10);
	endTranslate();
	startTranslate(0, 4, -5);
	glutSolidCube(4);
	endTranslate();


}
#ifdef __SHADOW

void renderScene(void)
{

	//First step: Render from the light POV to a FBO, story depth values only
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);	//Rendering offscreen

	//Using the fixed pipeline to render to the depthbuffer
	glUseProgramObjectARB(0);

	// In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
	glViewport(0, 0, SHADOW_MAP_WIDTH * SHADOW_MAP_RATIO, SHADOW_MAP_HEIGHT* SHADOW_MAP_RATIO);

	// Clear previous frame values
	glClear(GL_DEPTH_BUFFER_BIT);

	//Disable color rendering, we only want to write to the Z-Buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	setupMatrices(p_light[0], p_light[1], p_light[2], l_light[0], l_light[1], l_light[2]);

	// Culling switching, rendering only backface, this is done to avoid self-shadowing
	glCullFace(GL_FRONT);
	drawObjects();

	//Save modelview/projection matrice into texture7, also add a biais
	setTextureMatrix();


	// Now rendering from the camera POV, using the FBO to generate shadows
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

	//Enabling color write (previously disabled for light POV z-buffer rendering)
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Clear previous frame values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Using the shadow shader
	glUseProgramObjectARB(shadowShaderId);
	glUniform1iARB(shadowMapUniform, 7);
	glActiveTextureARB(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, depthTextureId);






	setupMatrices(p_camera[0], p_camera[1], p_camera[2], l_camera[0], l_camera[1], l_camera[2]);

	glCullFace(GL_BACK);
	drawObjects();

	// DEBUG only. this piece of code draw the depth buffer onscreen
	/*
	glUseProgramObjectARB(0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-SHADOW_MAP_WIDTH/2,SHADOW_MAP_WIDTH/2,-SHADOW_MAP_HEIGHT/2,SHADOW_MAP_HEIGHT/2,1,20);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor4f(1,1,1,1);
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,depthTextureId);
	glEnable(GL_TEXTURE_2D);
	glTranslated(0,0,-1);
	glBegin(GL_QUADS);
	glTexCoord2d(0,0);glVertex3f(0,0,0);
	glTexCoord2d(1,0);glVertex3f(SHADOW_MAP_WIDTH/2,0,0);
	glTexCoord2d(1,1);glVertex3f(SHADOW_MAP_WIDTH/2,SHADOW_MAP_HEIGHT/2,0);
	glTexCoord2d(0,1);glVertex3f(0,SHADOW_MAP_HEIGHT/2,0);


	glEnd();
	glDisable(GL_TEXTURE_2D);
	*/

	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27)
		exit(0);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
	glutCreateWindow("GLSL Shadow mapping");

	// This call will grab openGL extension function pointers.
	// This is not necessary for macosx and linux
	/*
	#ifdef _WIN32
	getOpenGLFunctionPointers();
	#endif
	*/
	glewInit();
	generateShadowFBO();
	loadShadowShader();

	// This is important, if not here, FBO's depthbuffer won't be populated.
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1.0f);

	glEnable(GL_CULL_FACE);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);




	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);

	glutKeyboardFunc(processNormalKeys);

	glutMainLoop();
}

#endif