//#define _SIMPLE_CUBE_
#include "stdafx.h"
#include <cassert>

#include "auxiliary.h"
#include "Render.h"
#include "world.h"
#include "lodepng.h"
#include "shadow.h"
#include "cursor.h"
#include "config.h"

extern World world;
extern Cursor cursor;
extern block_and_face seen_block;
extern Entity observer;

extern int idle_count, wWidth, wHeight;
const static float color_list[10][4] = { { 0, 0, 0, 1 }, { 1, 1, 1, 1 }, { 1, 1, 1, 1 }, { 1, 0, 0, 1 }, { 0, 0, 1, 1 }, { 0, 1, 1, 1 }, { 0, 1, 0, 1 }, { 0.3, 0.3, 0.3, 1 }, { 1, 1, 1, 1 }, { 0.5, 0.5, 0.5, 1 } };
const static float emission[10][4] = { {1,1,1,1}, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 } };
//                                 0       1       2       3       4       5       6       7
const static int point[8][3] = {{0,0,0},{0,1,0},{1,1,0},{1,0,0},{0,0,1},{0,1,1},{1,1,1},{1,0,1}};
const static int face[6][4] = {{1,5,6,2},{0,3,7,4},{3,2,6,7},{4,5,1,0},{7,6,5,4},{0,1,2,3}};// top->down->right->left->front->back
const static int t_point[4][2] = {{0,0},{0,1},{1,1},{1,0}};

Render render;
GLuint tex=1;

unsigned char *Render::LoadBitmapFile(const char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;	// 文件指针
	BITMAPFILEHEADER bitmapFileHeader;	// bitmap文件头
	unsigned char	*bitmapImage;		// bitmap图像数据
	int	imageIdx = 0;		// 图像位置索引
	unsigned char	tempRGB;	// 交换变量

	// 以“二进制+读”模式打开文件filename
	fopen_s(&filePtr, filename, "rb");
	if (filePtr == NULL) return NULL;
	// 读入bitmap文件图
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	// 验证是否为bitmap文件
	if (bitmapFileHeader.bfType != BITMAP_ID) {
		fprintf(stderr, "Error in LoadBitmapFile: the file is not a bitmap file\n");
		return NULL;
	}

	// 读入bitmap信息头
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	// 将文件指针移至bitmap数据
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	// 为装载图像数据创建足够的内存
	bitmapImage = new unsigned char[bitmapInfoHeader->biSizeImage];
	// 验证内存是否创建成功
	if (!bitmapImage) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	// 读入bitmap图像数据
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);
	// 确认读入成功
	if (bitmapImage == NULL) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	//由于bitmap中保存的格式是BGR，下面交换R和B的值，得到RGB格式
	for (imageIdx = 0;
	 imageIdx < bitmapInfoHeader->biSizeImage; imageIdx+=3) {
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}
	// 关闭bitmap图像文件
	fclose(filePtr);
	return bitmapImage;
}

void Render::texLoadPNG(int i, const char *filename)
{
	unsigned error;
	unsigned char* image;
	unsigned width, height;
	error = lodepng_decode32_file(&image, &width, &height, filename);
	if (error) {
		printf("error %u when opening %s : %s\n", error, filename, lodepng_error_text(error));
		return;
	}
	glBindTexture(GL_TEXTURE_2D, texture[i]);
	// 指定当前纹理的放大/缩小过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,
		0, 			//mipmap层次(通常为，表示最上层)
		GL_RGBA,	//RGBA
		width, //纹理宽带，必须是n，若有边框+2
		height, //纹理高度，必须是n，若有边框+2
		0, //边框(0=无边框, 1=有边框)
		GL_RGBA,	//bitmap数据的格式
		GL_UNSIGNED_BYTE, //每个颜色数据的类型
		image);	//bitmap数据指针
	//free(image);
}

void Render::texload(int i,const char *filename)
{

    BITMAPINFOHEADER bitmapInfoHeader;                                 // bitmap信息头
	unsigned char*   bitmapData;                                       // 纹理数据

	bitmapData = LoadBitmapFile(filename, &bitmapInfoHeader);
	if (bitmapData == NULL) {
		fprintf(stderr, "%s not loaded!\n", filename);
		fflush(stderr);
	}

	glBindTexture(GL_TEXTURE_2D, texture[i]);
	// 指定当前纹理的放大/缩小过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D,
		0, 	    //mipmap层次(通常为，表示最上层)
		GL_RGB,	//我们希望该纹理有红、绿、蓝数据
		bitmapInfoHeader.biWidth, //纹理宽带，必须是n，若有边框+2
		bitmapInfoHeader.biHeight, //纹理高度，必须是n，若有边框+2
		0, //边框(0=无边框, 1=有边框)
		GL_RGB,	//bitmap数据的格式
		GL_UNSIGNED_BYTE, //每个颜色数据的类型
		bitmapData);	//bitmap数据指针
}

void Render::set(int k)
{
	glBindTexture(GL_TEXTURE_2D,texture[k]);
  	glPixelStorei(GL_UNPACK_ALIGNMENT,1); //设置像素存储模式控制所读取的图像数据的行对齐方式.
    glTexImage2D(GL_TEXTURE_2D,0,3,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,tex_check);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
}

void Render::init()
{
	setTextureState(false);
#ifndef _SIMPLE_CUBE_
	setTextureState(true);
	glGenTextures(8, texture);
	texLoadPNG(0,"texture/sun.png");
	texload(1,"texture/3.bmp");
	texload(2,"texture/4.bmp");
	texload(3,"texture/5.bmp");
	texLoadPNG(4,"texture/gold_block.png");
	texLoadPNG(5,"texture/tallgrass.png");
	texload(6, "texture/6.bmp");
	texLoadPNG(7, "texture/cobblestone.png");
#endif
}

//use this function to start a series of transformation
void Render::beginTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glPushMatrix();
}

//use this function to end a series of transformation
void Render::endTransform()
{
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

//load in the identity matrix
void Render::loadIdentity(){
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//rotate the following objects angle degree along the axis
//need to be inside beginTransform() and endTransform
void Render::rotate(flt angle, Vec3f axis){
	glMatrixMode(GL_MODELVIEW);
	glRotatef(angle, axis[0], axis[1], axis[2]);
	glMatrixMode(GL_TEXTURE);
	glRotatef(angle, axis[0], axis[1], axis[2]);
}

//translate the following objects along vector p
//need to be inside beginTransform() and endTransform
void Render::translate(Vec3f p){
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(p[0], p[1], p[2]);
	glMatrixMode(GL_TEXTURE);
	glTranslatef(p[0], p[1], p[2]);
}

//scale the following objects each by vector p
//need to be inside beginTransform() and endTransform
void Render::scale(Vec3f p){
	glMatrixMode(GL_MODELVIEW);
	glScalef(p[0], p[1], p[2]);
	glMatrixMode(GL_TEXTURE);
	glScalef(p[0], p[1], p[2]);
}

void Render::renderCube(int type,int state)
{
#ifdef _SIMPLE_CUBE_
	beginTranslate(Vec3f(0.5, 0.5, 0.5));
	glutSolidCube(1.0);
	endTransform();
#else
	if (bTexture) {
		//glActiveTextureARB(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture[type]);
	}

	glBegin(GL_QUADS);
	for (int i = 0; i<6; i++)if (state>>i&1){
		glNormal3f(FACE[i][0], FACE[i][1], FACE[i][2]);
		for(int j=0;j<4;j++)
		{
			int p=face[i][j];
			glTexCoord2i(t_point[j][0],t_point[j][1]);
			glVertex3i(point[p][0],point[p][1],point[p][2]);
		}
	}
	glEnd();

#endif
}

void Render::renderObserver(Entity observer, flt r, flt h){
	GLUquadricObj *objCylinder = gluNewQuadric();
	beginTransform();
	translate(observer);
	rotate(-90, Vec3f(1, 0, 0));
	gluCylinder(objCylinder, r, r, h, 30, 10);
	endTransform();
}

void renderSeenBlock(block_and_face seen_block){
	if (seen_block.second == -1) return;
	render.beginTransform();
	render.translate(Vec3f(seen_block.first)+0.5);
	glutWireCube(1.01);
	render.endTransform();
}

void Render::renderScene(){
	for (auto it = world.begin(); it != world.end(); ++it){
		Vec3i p = it->first;
		beginTransform();
		translate(p);
		int msk = 0;
		for (int i = 0; i < 6; ++i)
			if (world.find(p + FACE[i]) == world.end())
				msk |= 1 << i;
		int b = it->second->get_block_type();
		use_material(color_list[b], color_list[b], NULL, 8);
		//if (b == 0) cout << "SUN!" << endl;
		render.renderCube(b, msk);
		endTransform();
	}
}

GLint tableList;
GLint genTableList(){
	GLint lid = glGenLists(1);
	glNewList(lid, GL_COMPILE);
	render.renderScene();
	glEndList();
	return lid;
}
void regenTableList(GLint lid){
	glNewList(lid, GL_COMPILE);
	render.renderScene();
	glEndList();
}

void renderInfo(Entity observer)
{
	static int frame = 0, time, timebase = 0;
	static char buffer[256];

	char mode[64];
	sprintf_s<64>(mode, " (%.4f,%.4f,%.4f)", observer[0], observer[1], observer[2]);

	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		sprintf_s<256>(buffer, "FPS:%4.2f %s IPS:%4.2f",
			frame*1000.0 / (time - timebase), mode, idle_count*1000.0 / (time - timebase));
		timebase = time;
		frame = 0;
		idle_count = 0;
	}

	char *c;
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);// 选择投影矩阵
	glPushMatrix();// 保存原矩阵
	glLoadIdentity();// 装入单位矩阵
	glOrtho(0, 480, 0, 480, -1, 1);// 位置正投影
	glMatrixMode(GL_MODELVIEW);// 选择Modelview矩阵
	glPushMatrix();// 保存原矩阵
	glLoadIdentity();// 装入单位矩阵*/
	glRasterPos2f(10, 10);
	for (c = buffer; *c != '\0'; c++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	glMatrixMode(GL_PROJECTION);// 选择投影矩阵
	glPopMatrix();// 重置为原保存矩阵
	glMatrixMode(GL_MODELVIEW);// 选择Modelview矩阵
	glPopMatrix();// 重置为原保存矩阵
	glEnable(GL_DEPTH_TEST);
}

void renderTableList(){
	if (world.changed) {
		regenTableList(tableList);
		world.changed = false;
	}
	glCallList(tableList);
}

void renderCross(){
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);// 选择投影矩阵
	glPushMatrix();// 保存原矩阵
	glLoadIdentity();// 装入单位矩阵
	glOrtho(0, wWidth, 0, wHeight, -1, 1);// 位置正投影
	glMatrixMode(GL_MODELVIEW);// 选择Modelview矩阵
	glPushMatrix();// 保存原矩阵
	glLoadIdentity();// 装入单位矩阵*/
	glRasterPos2d(wWidth/2-6, wHeight/2-6);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '+');
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void renderGUI(Entity observer){
	renderInfo(observer);
	renderCross();
}

void Render::setTextureState(bool bTex){
	bTexture = bTex;
	if (bTex) glEnable(GL_TEXTURE_2D);
	else glDisable(GL_TEXTURE_2D);
}

extern flt r, h;
extern bool bObserver;
void renderSceneDynamic(Entity observer){
	if (bObserver)
		render.renderObserver(observer, r, h);
}

void Render::renderBoxLine(){
	Vec3f p = floor(observer);
	glLineWidth(2.0);
	glColor3d(1, 1, 1);
	const int rg = 5;
	for (int i = -rg; i <= rg; ++i){
		for (int j = -rg; j <= rg; ++j){
			for (int k = -rg; k <= rg; ++k){
				beginTransform();
				translate(p+Vec3f(i+0.5, j+0.5, k+0.5));
				glutWireCube(1.0);
				endTransform();
			}
		}
	}
}

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


	extern flt light_pos[3];
	setupPerspective(light_pos, render.eye, true, true);

	// Culling switching, rendering only backface, this is done to avoid self-shadowing
#ifdef CULL_BACK
	glCullFace(GL_BACK);
#else
	glCullFace(GL_FRONT);
#endif

	render.setTextureState(false);
	//render the scene
	renderSceneDynamic(observer);
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

	extern GLhandleARB shader_id;
	//Using the shadow shader
	glUseProgramObjectARB(shader_id);
	glUniform1iARB(shadow_map_loc, 7);

	//Bind shadow depth as texture
	glActiveTextureARB(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, depth_texture_id);

	//Bind ordinary texture
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, render.texture[tex]);
	setupPerspective(render.eye, render.center, false, false);

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	extern bool bWire;
	if (bWire) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	render.setTextureState(true);
	//render the scene
	renderSceneDynamic(observer);
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
	
	renderGUI(observer);

	//Draw the GUI
	glUseProgramObjectARB(0);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	if (bBoxLine) {
		render.renderBoxLine();
	}
	glColor3d(0, 0, 0);
	renderSeenBlock(seen_block);

	extern bool bDebugDepthMap;
	// DEBUG only. this piece of code draw the depth buffer onscreen
	if (bDebugDepthMap) {
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

void Render::update_center(Cursor &cursor){
	cursor.update_facing_vector();
	Vec3f p_eye = observer.get_pos() + Vec3f(0, h_eye, 0);
	switch (view_mode) {
	case VIEW_MODE_FIRST_PERSON:
		eye = p_eye;
		center = eye + cursor.face;
		break;
	case VIEW_MODE_THIRD_PERSON_FRONT:
		center = p_eye;
		eye = p_eye + cursor.face * observer_dist;
		break;
	case VIEW_MODE_THIRD_PERSON_BACK:
		center = p_eye;
		eye = p_eye - cursor.face * observer_dist;
		break;
	}
	seen_block = world.look_at_block(p_eye, cursor.face, 10.0);
}