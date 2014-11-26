//#define _SIMPLE_CUBE_
#include "Render.h"
#include "vec.h"
#include "world.h"
#include "auxiliary.h"
#include <cassert>

extern World world;
extern int idle_count, wWidth, wHeight;
//                                 0       1       2       3       4       5       6       7
const static int point[8][3] = {{0,0,0},{0,1,0},{1,1,0},{1,0,0},{0,0,1},{0,1,1},{1,1,1},{1,0,1}};
const static int face[6][4] = {{1,5,6,2},{0,3,7,4},{3,2,6,7},{4,5,1,0},{7,6,5,4},{0,1,2,3}};// top->down->right->left->front->back
const static int t_point[4][2] = {{0,0},{0,1},{1,1},{1,0}};

Render render;
GLuint tex;

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
    glTexImage2D(GL_TEXTURE_2D,0,3,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,tex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
}

void Render::init()
{
	glGenTextures(7, texture);
	texload(0,"texture/Monet.bmp");
    texload(1,"texture/Crack.bmp");
	texload(2,"texture/3.bmp");
	texload(3,"texture/4.bmp");
	texload(4,"texture/5.bmp");
	texload(5,"texture/6.bmp");
	for(int i=0;i<256;i++)
		for(int j=0;j<256;j++){
			int t=(i/64)+(j/64);
			int k=255;
			if(t & 1) k=0;
			for(int l=0;l<=2;l++) tex[i][j][l]=k;
		}
	set(6);
}

void Render::draw_Cube(int type,int state)
{
#ifdef _SIMPLE_CUBE_
	glPushMatrix();
	glTranslatef(0.5, 0.5, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();
#else
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[type]);
	glBegin(GL_QUADS);
	for(int i=0;i<6;i++)
	{
		int t=(state>>i)&1;
		if(t==0) continue;
		glNormal3f(FACE[i][0], FACE[i][1], FACE[i][2]);
		for(int j=0;j<4;j++)
		{
			int p=face[i][j];
			glTexCoord2i(t_point[j][0],t_point[j][1]);
			glVertex3i(point[p][0],point[p][1],point[p][2]);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
#endif
}

void DrawObserver(Entity observer, flt r, flt h){
	GLUquadricObj *objCylinder = gluNewQuadric();
	glPushMatrix();
	glTranslatef(observer[0], observer[1], observer[2]);
	gluCylinder(objCylinder, r, r, h, 30, 10);
	glPopMatrix();
}

void DrawSeenBlock(block_and_face seen_block){
	if (seen_block.second == -1) return;
	static flt p[3];
	for (int i = 0; i < 3; ++i)
		p[i] = seen_block.first[i] + 0.5;
	glPushMatrix();
	glTranslatef(p[0], p[1], p[2]);
	glutWireCube(1.01);
	glPopMatrix();
}

void Draw_Scene(){
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	use_material(golden, white, NULL, 2);
	glTranslatef(0, 4, 0);
	glRotatef(180, 1, 0, 0);
	glutSolidTeapot(-1.0);
	glPopMatrix();
	use_material(white, black, NULL, 1);
	for (auto it = world.begin(); it != world.end(); ++it){
		glPushMatrix();
		Vec3i p = it->first;
		glTranslatef(p[0], p[1], p[2]);
		for (int i = 0; i < 6; ++i)
			if (world.find(p + FACE[i]) == world.end())
				render.draw_Cube(tex, 1 << i);
		glPopMatrix();
	}
}

GLint tableList;
GLint genTableList(){
	GLint lid = glGenLists(1);
	glNewList(lid, GL_COMPILE);
	Draw_Scene();
	glEndList();
	return lid;
}
void regenTableList(GLint lid){
	glNewList(lid, GL_COMPILE);
	Draw_Scene();
	glEndList();
}

void drawInfo(Entity observer)
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

	//glutSetWindowTitle(buffer);
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
	for (c = buffer; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	}
	glMatrixMode(GL_PROJECTION);// 选择投影矩阵
	glPopMatrix();// 重置为原保存矩阵
	glMatrixMode(GL_MODELVIEW);// 选择Modelview矩阵
	glPopMatrix();// 重置为原保存矩阵
	glEnable(GL_DEPTH_TEST);
}

void drawTableList(){
	glCallList(tableList);
}

void drawCross(){
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);// 选择投影矩阵
	glPushMatrix();// 保存原矩阵
	glLoadIdentity();// 装入单位矩阵
	glOrtho(0, wWidth, 0, wHeight, -1, 1);// 位置正投影
	glMatrixMode(GL_MODELVIEW);// 选择Modelview矩阵
	glPushMatrix();// 保存原矩阵
	glLoadIdentity();// 装入单位矩阵*/
	glRasterPos2d(wWidth / 2, wHeight / 2);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '+');
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void drawGUI(Entity observer){
	drawInfo(observer);
	drawCross();
}