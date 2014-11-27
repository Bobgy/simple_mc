#define _CRT_SECURE_NO_WARNINGS
#define _ENABLE_CUSTOM_SHADERS_
//#define _MOVING_LIGHT_
#define CULL_BACK
#pragma comment(lib, "glew32.lib")

#include "stdafx.h"

#include <math.h>
#include <map>
#include <array>
#include <ctime>
#include <string>
#include <cassert>
#include "world.h"
#include "entity.h"
#include "render.h"
#include "KeyBoardControl.h"
#include "cursor.h"
#include "shader.h"
#include "auxiliary.h"
#include "shadow.h"
#include "screenshot.h"

using namespace std;

GLfloat light_pos[] = { 1.9, 1.0, 0.5, 0 };
//GLfloat light_pos[] = { 4.5, 4.5, 4.5, 0.2 };

float fTranslate;
float fRotate;
float fScale     = 1.0f;	// set inital scale value to 1.0f

bool bPersp = true;
bool bAnim = false;
bool bWire = false;
bool bGravity = false;

int wHeight =  724;
int wWidth = 1024;
extern GLuint tex;
GLhandleARB shader_id;

float r = 0.45, h = 1.6;
typedef Vec3i Pt3;
World world(time(NULL), 13);
block_and_face seen_block = make_pair(Vec3i(), -1);
extern Render render;
extern KeyboardControl keyboard;
extern Cursor cursor;

flt pp[3] = { 0, 10, 0 }, vv[3] = { 0, 0, 0 };
Entity observer(pp, vv, r, h, 1.0);
float center[] = { 0, 4, 0 };
float eye[] = { 0, 4, 8, 1};
float PI = acos(-1.0), deg2rad = PI / 180.0;
flt face[] = { 1, 0, 0 };
flt face_xz[] = { 1, 0, 0 };
int windowHandle;

bool enableObserver = 0;

int screenshotCount = 0;

void renderSceneDynamic(){
	if (enableObserver)
		renderObserver(observer, r, h);
}

void updateView()
{
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	float whRatio = (GLfloat)wWidth/(GLfloat)wHeight;
	
	gluPerspective(45, whRatio, 0.1, 300);
	glViewport(0, 0, wWidth, wHeight);					// Reset The Current Viewport

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

void reshape(int width, int height)
{
	if (height==0) height=1;

	wHeight = height;
	wWidth = width;

	updateView();
}

void update_dir(){
	float ha = cursor.get_horizontal_angle(),
		  va = cursor.get_vertical_angle();
	face_xz[0] = cos(ha);
	face_xz[2] = sin(ha);
	face_xz[1] = 0;
	face[0] = face_xz[0] * cos(va);
	face[2] = face_xz[2] * cos(va);
	face[1] = sin(va);
}

float dis = 1.0;

void update_center(){
	update_dir();
	for (int i = 0; i < 3; ++i){
		eye[i] = observer[i] + (i == 1 ? h*0.8 : 0);
		center[i] = eye[i] + face[i] * dis;
	}
	seen_block = world.look_at_block(Vec3f(eye[0],eye[1],eye[2]), Vec3f(face[0],face[1],face[2]), 10.0);
}

flt step = 0.3, eps = 1e-8;
int idle_count = 0;
clock_t lst = 0, inter = CLOCK_T * CLOCKS_PER_SEC;
void idle()
{
	++idle_count;
	static clock_t now;
	now = clock();
	
#ifdef _MOVING_LIGHT_
	flt ang = glutGet(GLUT_ELAPSED_TIME) / 2000.0;
	light_pos[0] = cos(ang);
	light_pos[1] = sin(ang);
	light_pos[2] = 1.0;
#endif

	if (lst + inter <= now) {
		if(bGravity)observer.fall();
		int df = 0;
		static flt ff[3];
		if (keyboard.get_state('c')){
			std::string filename = "screenshot_"+std::to_string(screenshotCount) +".bmp";
			ScreenShot(filename);
			screenshotCount++;
		}
		if (keyboard.get_state('w') ^ keyboard.get_state('s')){
			df = keyboard.get_state('w') ? 1 : -1;
			observer.give_velocity(face_xz, step*df);
		}
		if (keyboard.get_state('a') ^ keyboard.get_state('d')){
			df = keyboard.get_state('a') ? 1 : -1;
			ff[0] = face_xz[2];
			ff[1] = 0;
			ff[2] = -face_xz[0];
			observer.give_velocity(ff, step*0.5*df);
		}
		if (bGravity){
			if (keyboard.get_state(' ') && observer.on_ground){
				observer.force(Vec3f(0, 13, 0));
			}
		} else {
			if (keyboard.get_state(' '))
				observer.give_velocity(Vec3f(0, 0.5, 0), 1);
			if (keyboard.get_state('g'))
				observer.give_velocity(Vec3f(0, -0.5, 0), 1);
		}
		static int p[3];
		for (int i = 0; i < 3; ++i)
			p[i] = floor(observer[i]);
		p[1] = floor(observer[1] - 0.5*h);
		observer.on_ground = false;
		for (int dx = -1; dx <= 1; ++dx){
			for (int dy = -2; dy <= 2; ++dy){
				for (int dz = -1; dz <= 1; ++dz){
					static map<Pt3, Block*>::const_iterator it;
					if ((it = world.find(Pt3({ p[0] + dx, p[1] + dy, p[2] + dz }))) != world.end()){
						observer.collide_cube_horizontally(it->first);
						observer.on_ground |= observer.collide_cube_vertically(it->first);
					}
				}
			}
		}
		if (observer.on_ground) observer.be_slowed(0.90);
		observer.update();
		update_center();
		updateView();
		lst += inter;
	}
	glutPostRedisplay();
}


extern int tableList;

void DisplayScene(){
	//First step: Render from the light POV to a FBO, story depth values only
	extern GLuint fboId;
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);	//Rendering offscreen

	//Using the fixed pipeline to render to the depthbuffer
	glUseProgramObjectARB(0);

	// In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
	glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

	// Clear previous frame values
	glClear(GL_DEPTH_BUFFER_BIT);

	//Disable color rendering, we only want to write to the Z-Buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	setupMatrices(light_pos, eye, true);

	// Culling switching, rendering only backface, this is done to avoid self-shadowing
#ifdef CULL_BACK
	glCullFace(GL_BACK);
#else
	glCullFace(GL_FRONT);
#endif

	render.setTextureState(false);
	//render the scene
	renderSceneDynamic();
	//renderTableList();
	render.renderScene();

	//Save modelview/projection matrice into texture7, also add a biais
	setTextureMatrix();

	// Now rendering from the camera POV, using the FBO to generate shadows
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glViewport(0, 0, wWidth, wHeight);

	//Enabling color write (previously disabled for light POV z-buffer rendering)
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Clear previous frame values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	extern GLuint shadow_map_loc, time_loc, depth_texture_id;

	// Clear the color to be sky blue
	glClearColor(0.0f, 0.5f, 1.0f, 0.0f);

	//Using the shadow shader
	glUseProgramObjectARB(shader_id);
	glUniform1iARB(shadow_map_loc, 7);

	//Bind shadow depth as texture
	glActiveTextureARB(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, depth_texture_id);

	//Bind ordinary texture
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, render.texture[tex]);
	setupMatrices(eye, center, false);
	//setupMatrices(light_pos, eye, true);

	glCullFace(GL_BACK);
	if (bWire) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	render.setTextureState(false);
	//render the scene
	renderSceneDynamic();
	//renderTableList();
	render.renderScene();

	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, dark_grey);

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	int state = 7;
	glLightfv(GL_LIGHT0, GL_AMBIENT, (state & 1) ? grey : black);
	glLightfv(GL_LIGHT0, GL_SPECULAR, (state & 2) ? white : black);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, (state & 4) ? light_grey : black);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.00);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00);
	glEnable(GL_LIGHT0);

	renderSeenBlock(seen_block);
	renderGUI(observer);

	if (bDebugDepthMap) {
		// DEBUG only. this piece of code draw the depth buffer onscreen
		glUseProgramObjectARB(0);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, wWidth, 0, wHeight, 1, 200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glColor4f(1, 1, 1, 1);
		glActiveTextureARB(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depth_texture_id);
		glEnable(GL_TEXTURE_2D);
		glTranslated(0, 0, -1);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3f(wWidth / 2.0, wHeight / 2.0, 0);
		glTexCoord2d(1, 0); glVertex3f(wWidth, wHeight / 2.0, 0);
		glTexCoord2d(1, 1); glVertex3f(wWidth, wHeight, 0);
		glTexCoord2d(0, 1); glVertex3f(wWidth / 2.0, wHeight, 0);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	glutSwapBuffers();
}

void init(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(wWidth, wHeight);
	windowHandle = glutCreateWindow("Simple MC");
	
	glewInit();
	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
	if (glewIsSupported("GL_VERSION_2_0"))
		printf("Ready for OpenGL 2.0\n");
	else {
		printf("OpenGL 2.0 not supported\n");
		exit(1);
	}
	generateShadowFBO();

	keyboard.init();
	render.init();
	tableList = genTableList();

	init_cursor();
	glutDisplayFunc(DisplayScene);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	glLineWidth(3.0);
	glEnable(GL_CULL_FACE);
	glutSetCursor(GLUT_CURSOR_NONE);

#ifdef _ENABLE_CUSTOM_SHADERS_
	shader_id = getShaders();
#endif
}


int main (int argc,  char *argv[])
{
	init(argc, argv);
	glutMainLoop();
	return 0;
}