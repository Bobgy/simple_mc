//#define _SIMPLE_CUBE_
#include <stdafx.h>
#include <cassert>
#include <list>

#include "render/auxiliary.h"
#include "render/render.h"
#include "render/shadow.h"

#include "game/game.h"
#include "game/world.h"
#include "game/item.h"
#include "game/block.h"
#include "game/entity.h"

#include "utility/lodepng.h"
#include "utility/view.h"
#include "utility/config.h"
#include "utility/keyboard.h"
#include "utility/obj.h"

extern BlockAndFace seen_block;
extern list<Item> items;
//ObjModel obj;

extern int idle_count, wWidth, wHeight;
const static float color_list[10][4] = { { 0, 0, 0, 1 }, { 1, 1, 1, 1 }, { 1, 1, 1, 1 }, { 1, 0, 0, 1 }, { 0, 0, 1, 1 }, { 0, 1, 1, 1 }, { 0, 1, 0, 1 }, { 0.3, 0.3, 0.3, 1 }, { 1, 1, 1, 1 }, { 0.5, 0.5, 0.5, 1 } };
const static float emission[10][4] = { {1,1,1,1}, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 } };

const static int point[8][3] = {
		{ 0, 0, 0 },//0
		{ 0, 1, 0 },//1
		{ 1, 1, 0 },//2
		{ 1, 0, 0 },//3
		{ 0, 0, 1 },//4
		{ 0, 1, 1 },//5
		{ 1, 1, 1 },//6
		{ 1, 0, 1 } //7
};
const static int face[6][4] = {
		{ 5, 6, 2, 1 },// top
		{ 0, 3, 7, 4 },// down
		{ 3, 2, 6, 7 },// front
		{ 1, 0, 4, 5 },// back
		{ 5, 4, 7, 6 },// right
		{ 2, 3, 0, 1 } // left
};
const static int t_point[4][2] = {{0,0},{0,1},{1,1},{1,0}};

Render render;
GLuint tex=1;

unsigned char *Render::LoadBitmapFile(const char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;
	BITMAPFILEHEADER bitmapFileHeader; // bitmap文件头
	unsigned char	*bitmapImage;	   // bitmap图像数据
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

void Render::texLoad(int i,const char *filename)
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
	glGenTextures(20, texture);
	texLoad(0, "texture/white.bmp");
	texLoad(1, "texture/3.bmp");
	texLoad(2, "texture/4.bmp");
	texLoad(3, "texture/5.bmp");
	texLoadPNG(4, "texture/gold_block.png");
	texLoadPNG(5, "texture/tallgrass.png");
	texLoad(6, "texture/6.bmp");
	texLoadPNG(7, "texture/cobblestone.png");
	texLoadPNG(19, "texture/steve.png");
	texLoadPNG(10, "texture/log.png");
	texLoadPNG(11, "texture/log_top.png");
	texLoadPNG(12, "texture/0.png");
	texLoadPNG(13, "texture/1.png");
	texLoadPNG(14, "texture/2.png");
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
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glPopMatrix();
}

//load in the identity matrix
void Render::loadIdentity(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glLoadIdentity();
}

//rotate the following objects angle degree along the axis
//need to be inside beginTransform() and endTransform
void Render::rotate(flt angle, Vec3f axis){
	glMatrixMode(GL_MODELVIEW);
	glRotatef(angle, axis[0], axis[1], axis[2]);
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glRotatef(angle, axis[0], axis[1], axis[2]);
}

//translate the following objects along vector p
//need to be inside beginTransform() and endTransform
void Render::translate(Vec3f p){
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(p[0], p[1], p[2]);
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glTranslatef(p[0], p[1], p[2]);
}

//scale the following objects each by vector p
//need to be inside beginTransform() and endTransform
void Render::scale(Vec3f p){
	glMatrixMode(GL_MODELVIEW);
	glScalef(p[0], p[1], p[2]);
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glScalef(p[0], p[1], p[2]);
}

void Render::renderCube(int type,int state)
{
#ifdef _SIMPLE_CUBE_
	glutSolidCube(1.0);
#else
	if (bTexture) {
		glActiveTextureARB(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[type]);
	}
	glBegin(GL_QUADS);
	for (int i = 0; i<6; i++)if (state>>i&1){
		glNormal3f(FACE[i][0], FACE[i][1], FACE[i][2]);
		for(int j=0;j<4;j++)
		{
			int p=face[i][j];
			glTexCoord2i(t_point[j][0],t_point[j][1]);
			glVertex3f(point[p][0]-0.5,point[p][1]-0.5,point[p][2]-0.5);
		}
	}
	glEnd();
#endif
}

void Render::renderCubeTex(int type, const CubeTexCoord &tex)
{
#ifdef _SIMPLE_CUBE_
	glutSolidCube(1.0);
#else
	if (bTexture) {
		glActiveTextureARB(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[type]);
	}
	glBegin(GL_QUADS);
	for (int i = 0; i<6; i++){
		glNormal3f(FACE[i][0], FACE[i][1], FACE[i][2]);
		for (int j = 0; j<4; j++)
		{
			int p = face[i][j];
			glTexCoord2fv(tex.getCoord(i, j));
			glVertex3f(point[p][0] - 0.5, point[p][1] - 0.5, point[p][2] - 0.5);
		}
	}
	glEnd();
#endif
}

void Render::renderPlayer(const Entity &observer, flt r, flt h) {
	const Keyboard &keyboard = *CurrentGame()->getKeyboard();

	static flt ang = 0.f, delta = 4.33f, body_ang = 0.f;
	if (keyboard.is_walking()){
		ang += delta;
		if (ang > 45.f) {
			ang = 45.f;
			delta = -delta;
		}
		if (ang < -45.f) {
			ang = -45.f;
			delta = -delta;
		}
	}
	else {
		ang *= 0.96f;
	}

	beginTransform();
	translate(observer);
	const Rotation *rotations = observer.getRotation();
	rotate(-rotations->getH() * rad2deg, Vec3f(0, 1, 0));

	//begin Head
	beginTransform();
	static CubeTexCoord headTexCoord(Vec3i(16, 16, 16), 16, 16, 64, 128);
	translate(Vec3f(0, 1.35, 0));
	scale(Vec3f(0.4, 0.4, 0.4));
	rotate(rotations->getV() * 0.8 * rad2deg, Vec3f(0, 0, 1));
	renderCubeTex(texPlayer, headTexCoord);
	endTransform();
	//end Head

	beginTransform();
	if (keyboard.get_state('a') ^ keyboard.get_state('d')) {
		if (keyboard.get_state('a'))
			body_ang = (body_ang + 0.2f * 45.f) / 1.2f;
		else
			body_ang = (body_ang + 0.2f * -45.f) / 1.2f;
	}
	else body_ang *= 0.96f;
	rotate(body_ang, Vec3f(0.0f, 1.0f, 0.0f));

	//begin Body
	beginTransform();
	static CubeTexCoord bodyTexCoord(Vec3i(8, 24, 16), 40, 40, 64, 128);
	translate(Vec3f(0, 0.85, 0));
	scale(Vec3f(0.3, 0.6, 0.45));
	renderCubeTex(texPlayer, bodyTexCoord);
	endTransform();
	//end Body

	static CubeTexCoord armTexCoord(Vec3i(8, 24, 8), 40, 88, 64, 128);
	//begin Arm
	for (int sg : {-1, 1}){
		beginTransform();
		translate(Vec3f(0, 1.2, sg*0.3));
		rotate(sg*ang, Vec3f(0, 0, 1));
		rotate(sg * -5, Vec3f(1, 0, 0));
		scale(Vec3f(0.15, 0.7, 0.15));
		translate(Vec3f(0, -0.5, 0));
		renderCubeTex(texPlayer, armTexCoord);
		endTransform();
	}
	//end Arm

	static CubeTexCoord legTexCoord(Vec3i(8, 24, 8), 40, 8, 64, 128);
	//begin Leg
	for (int sg : {-1, 1}){
		beginTransform();
		translate(Vec3f(0, 0.7, sg*0.11));
		rotate(-sg*ang, Vec3f(0, 0, 1));
		scale(Vec3f(0.2, 0.8, 0.2));
		translate(Vec3f(0, -0.5, 0));
		renderCubeTex(texPlayer, legTexCoord);
		endTransform();
	}
	//end Leg

	endTransform();

	//CollisionBox
	if (bCollisionBox) {
		GLUquadricObj *objCylinder = gluNewQuadric();
		rotate(-90, Vec3f(1, 0, 0));
		glutWireCylinder(r, h, 30, 1);
	}
	endTransform();
}


void renderSeenBlock(BlockAndFace seen_block){
	if (seen_block.second == -1) return;
	render.beginTransform();
	render.translate(Vec3f(seen_block.first)+0.5);
	glutWireCube(1.01);
	render.endTransform();
}

void Render::renderScene(){
	const World &world = *CurrentGame()->getWorld();
	for (auto it = world.begin(); it != world.end(); ++it){
		Vec3i p = it->first;
		beginTransform();
		translate(Vec3f(p)+0.5);
		int msk = 0;
		for (int i = 0; i < 6; ++i)	{
			auto it = world.find(p + FACE[i]);
			if (it == world.end() || !it->second->is_opaque())
				msk |= 1 << i;
		}
		block_type b = it->second->get_block_type();
		use_material(white, white, black, black, 1);
		if (b == LOG) {
			render.renderCube(10, 0x3c & msk);
			render.renderCube(11, 3 & msk);
		} else if (b == LEAVES) {
			int mask[3] = { 0 };
			srand(((it->first[0] * 13) + it->first[1] * 13) + it->first[2]);
			for (int i = 0; i < 6; ++i)
				mask[rand() % 3] |= 1 << i;
			use_material(green, green, black, black, 1);
			for (int i = 0; i < 3; ++i)
				render.renderCube(12+i, mask[i] & msk);
		} else if (b == GOLD){
			use_material(golden, golden, white, black, 10);
			render.renderCube(b, msk);
		} else render.renderCube(b, msk);
		endTransform();
	}
}

GLint genTableList(){
	GLint lid = glGenLists(1);
	glNewList(lid, GL_COMPILE);
	render.renderScene();
	glEndList();
	return lid;
}
void regenTableList(GLint lid){
	glNewList(lid, GL_COMPILE);
	render.setTextureState(true);
	render.renderScene();
	glEndList();
}
void renderTableList(){
	World &world = *CurrentGame()->getWorld();
	if (world.changed) {
		regenTableList(tableList);
		world.changed = false;
	}
	glCallList(tableList);
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
	glMatrixMode(GL_MODELVIEW);// 选择Modelview矩阵
	glPushMatrix();// 保存原矩阵
	glLoadIdentity();// 装入单位矩阵*/
	glRasterPos2f(10, 10);
	for (c = buffer; *c != '\0'; c++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	glPopMatrix();// 重置为原保存矩阵
}

void renderCross(){
	glMatrixMode(GL_MODELVIEW);// 选择Modelview矩阵
	glPushMatrix();// 保存原矩阵
	glLoadIdentity();// 装入单位矩阵*/
	glRasterPos2f(wWidth/2-6, wHeight/2-6);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '+');
	glPopMatrix();
}

void renderGUI(Entity observer){
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);// 选择投影矩阵
	glPushMatrix();// 保存原矩阵
	glLoadIdentity();// 装入单位矩阵
	glOrtho(0, wWidth, 0, wHeight, -1, 1);// 位置正投影

	renderInfo(observer);
	renderCross();

	glMatrixMode(GL_PROJECTION);// 选择投影矩阵
	glPopMatrix();// 重置为原保存矩阵
	glEnable(GL_DEPTH_TEST);
}

void Render::setTextureState(bool bTex){
	bTexture = bTex;
	if (bTex) glEnable(GL_TEXTURE_2D);
	else glDisable(GL_TEXTURE_2D);
}

extern flt r, h;
void renderSceneDynamic(const Entity &observer){
	if (bObserver) {
		render.renderPlayer(observer, r, h);
	}
}

void Render::renderBoxLine(){
	Vec3f p = floor(CurrentGame()->getPlayerEntity()->get_pos());
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

extern GLhandleARB shader_id;
extern flt light_pos[4];
void display(){
	if (bCustomGLSL) {
		//First step: Render from the light POV to a FBO, store depth values only
		extern GLuint fboId;
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);	//Rendering offscreen

		//Using the fixed pipeline to render to the depthbuffer
		glUseProgramObjectARB(0);
		//glUseProgramObjectARB(shader_id);

		// In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
		glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

		// Clear previous frame values
		glClear(GL_DEPTH_BUFFER_BIT);

		//Disable color rendering, we only want to write to the Z-Buffer
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		Vec3f face_xz = CurrentGame()->getPlayerEntity()->getRotation()->getHorizontalFacingVector();
		// TODO
		render.setupPerspective(light_pos, render.eye + face_xz*0.4*VIEW_DISTANCE,
			face_xz^Vec3f(light_pos), true, true);

		// Culling switching, rendering only backface, this is done to avoid self-shadowing
		glEnable(GL_CULL_FACE);
#ifdef CULL_BACK
		glCullFace(GL_BACK);
#else
		glCullFace(GL_FRONT);
#endif

		render.setTextureState(false);
		//render the scene
		renderSceneDynamic(*CurrentGame()->getPlayerEntity());
		renderTableList();
		render.draw_item();
		//render.renderScene();

		//Save modelview/projection matrice into texture7, also add a biais
		setTextureMatrix();

		// Now rendering from the camera POV, using the FBO to generate shadows
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
	glViewport(0, 0, wWidth, wHeight);

	//Enabling color write (previously disabled for light POV z-buffer rendering)
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Clear previous frame values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	extern GLuint shadow_map_loc, time_loc, depth_texture_id;

	// Clear the color to be sky blue
	glClearColor(0.0f, 0.5f, 1.0f, 0.0f);

	if (bCustomGLSL){
		//Using the shadow shader
		glUseProgramObjectARB(shader_id);
		glUniform1iARB(shadow_map_loc, 7);
		//Bind shadow depth as texture
		glActiveTextureARB(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, depth_texture_id);
	}

	//Bind ordinary texture
	glActiveTextureARB(GL_TEXTURE0);

	render.setupPerspective(render.eye, render.center, Vec3f(0,1,0), false, false);
	//render.setupPerspective(light_pos, render.eye + cursor.face_xz*0.4*VIEW_DISTANCE,
	//							cursor.face_xz^Vec3f(light_pos), true, true);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glDisable(GL_CULL_FACE);

	extern bool bWire;
	if (bWire) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	render.setTextureState(true);
	//render the scene
	renderSceneDynamic(*CurrentGame()->getPlayerEntity());
	renderTableList();
	render.draw_item();
	//render.renderScene();

	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, dark_grey);

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	int state = 7;
	glLightfv(GL_LIGHT0, GL_AMBIENT, (state & 1) ? grey : black);
	glLightfv(GL_LIGHT0, GL_SPECULAR, (state & 2) ? white : black);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, (state & 4) ? white : black);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.00);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00);
	glEnable(GL_LIGHT0);

	//Draw the GUI
	glUseProgramObjectARB(0);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glColor3f(0, 0, 0);
	renderGUI(*CurrentGame()->getPlayerEntity());
	if (bBoxLine) render.renderBoxLine();
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

void Render::update_center(){
	shared_ptr<const Entity> entity = CurrentGame()->getPlayerEntity();
	Vec3f p_eye = entity->get_pos() + Vec3f(0, h_eye, 0);
	const Rotation *view = entity->getRotation();
	switch (view_mode) {
	case VIEW_MODE_FIRST_PERSON:
		eye = p_eye;
		center = eye + view->getFacingVector();
		break;
	case VIEW_MODE_THIRD_PERSON_FRONT:
		center = p_eye;
		eye = p_eye + view->getFacingVector() * observer_dist;
		break;
	case VIEW_MODE_THIRD_PERSON_BACK:
		center = p_eye;
		eye = p_eye - view->getFacingVector() * observer_dist;
		break;
	default:
		LOG_WARNING(__FUNCTION__, "unknown view_mode %d\n", view_mode);
	}
	seen_block = CurrentGame()->getWorld()->look_at_block(p_eye, view->getFacingVector(), 10.0);
}

void Render::setupPerspective(const Vec3f eye, Vec3f center, Vec3f up, bool lightSource, bool parallel)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	int width, height;
	if (lightSource) {
		width = SHADOW_MAP_WIDTH;
		height = SHADOW_MAP_HEIGHT;
	}
	else {
		extern int wWidth, wHeight;
		width = wWidth;
		height = wHeight;
	}
	int sz = lightSource ? VIEW_DISTANCE*0.8 : 10;
	if (!parallel) gluPerspective(45, width / flt(height), 0.1, VIEW_DISTANCE);
	else glOrtho(-sz, sz, -sz, sz, 0.1, 200);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vec3f neye = eye;
	if (parallel) neye = center + neye.normalize() * 50.0f;
	gluLookAt(neye[0], neye[1], neye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
}

void Render::draw_item()
{
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	for (auto x : items){
		if (x.draw) {
			beginTransform();
				translate(x.loc);
				scale(Vec3f(x.scale,x.scale,x.scale));
				x.obj.call_list();
			endTransform();
		}
	}
}