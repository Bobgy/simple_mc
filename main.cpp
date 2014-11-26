#define _CRT_SECURE_NO_WARNINGS
#define _ENABLE_CUSTOM_SHADERS_

#pragma comment(lib, "glew32.lib")

#include "stdafx.h"

#include <math.h>
#include <map>
#include <array>
#include <ctime>
#include <cassert>
#include "world.h"
#include "entity.h"
#include "render.h"
#include "KeyBoardControl.h"
#include "cursor.h"
#include "textfile.h"
#include "auxiliary.h"

using namespace std;

GLuint v, f, f2, p, loc;

float lpos[4] = { 1, 0.5, 1, 0 };

float fTranslate;
float fRotate;
float fScale     = 1.0f;	// set inital scale value to 1.0f

bool bPersp = true;
bool bAnim = false;
bool bWire = false;

int wHeight = 500;
int wWidth = 500;
int tex = 0;

float r = 0.45, h = 1.6;
bool pot_rotate=0;

typedef Vec3i Pt3;
World world(time(NULL));
block_and_face seen_block = make_pair(Vec3i(), -1);
Render render;
extern KeyboardControl keyboard;
extern Cursor cursor;

flt pp[3] = { 0, 10, 0 }, vv[3] = { 0, 0, 0 };
Entity observer(pp, vv, r, h, 1.0);
float center[] = { 0, 4, 0 };
float eye[] = { 0, 4, 8 };
float PI = acos(-1.0), deg2rad = PI / 180.0;
flt face[] = { 1, 0, 0 };
flt face_xz[] = { 1, 0, 0 };

float golden[] = { 0.85f, 0.65f, 0.2f, 1.0f };
float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float black[] = { 0, 0, 0, 1 };
float grey[] = { 0.5, 0.5, 0.5, 1 };

#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(char *file, int line)
{
	//
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	//
	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}

void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void setShaders() {

	char *vs = NULL, *fs = NULL, *fs2 = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	f2 = glCreateShader(GL_FRAGMENT_SHADER);
//#define SHADER "pixeldirdifambspec"
//#define SHADER "textureSimple"
//#define SHADER "lightPointwise"
#define SHADER "mine"

	vs = textFileRead("shader/"SHADER"_vertex.glsl");
	fs = textFileRead("shader/"SHADER"_fragment.glsl");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs); free(fs);

	glCompileShader(v);
	glCompileShader(f);

	printShaderInfoLog(v);
	printShaderInfoLog(f);
	printShaderInfoLog(f2);

	p = glCreateProgram();
	glAttachShader(p, v);
	glAttachShader(p, f);

	glLinkProgram(p);
	printProgramInfoLog(p);

	glUseProgram(p);
	loc = glGetUniformLocation(p, "time");

}

bool enableObserver = 0;
void DrawObserver(){
	if (!enableObserver) return;
	GLUquadricObj *objCylinder = gluNewQuadric();
	glPushMatrix();
	glTranslatef(observer[0], observer[1], observer[2]);
	//glRotatef(90, 1, 0, 0);
	glutSolidSphere(0.5, 20, 20);
	//gluCylinder(objCylinder, r, r, 1, 30, 10);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glutSolidSphere(0.5, 20, 20);
	//gluCylinder(objCylinder, r, r, 1, 30, 10);
	glPopMatrix();
}

void DrawSeenBlock(){
	if (seen_block.second == -1) return;
	static flt p[3];
	for (int i = 0; i < 3; ++i)
		p[i] = seen_block.first[i] + 0.5;
	glPushMatrix();
	glTranslatef(p[0], p[1], p[2]);
	glutWireCube(1.01);
	glPopMatrix();
}

void Draw_Scene_Dynamic(){
	DrawObserver();
	DrawSeenBlock();
}

void Draw_Scene()
{
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	use_material(golden, white, NULL, 2);
	glTranslatef(0, 4, 0);
	glutSolidTeapot(1);
	glPopMatrix();
	use_material(white, black, NULL, 1);
	for (auto it = world.begin(); it != world.end(); ++it){
		glPushMatrix();
		const Pt3 &p = it->first;
		glTranslatef(p[0], p[1], p[2]);
		for (int i = 0; i < 6; ++i)
			if (world.find(p + FACE[i]) == world.end())
				render.draw_Cube(tex, 1 << i);
		glPopMatrix();
	}
}

float kk = 0;

void updateView()
{
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	float whRatio = (GLfloat)wWidth/(GLfloat)wHeight;
	
	gluPerspective(40*exp(kk), whRatio, 0.1, 300);
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

float dis = 10.0;

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
	if (lst + inter <= now) {
		observer.fall();
		int df = 0;
		static flt ff[3];
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
		if (keyboard.get_state(' ') && observer.on_ground){
			observer.force(Vec3f(0, 13, 0));
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

GLint tableList;
GLint GenTableList()
{
	GLint lid = glGenLists(1);
	glNewList(lid, GL_COMPILE);
	Draw_Scene();
	glEndList();
	return lid;
}
void RegenTableList(GLint lid){
	glNewList(lid, GL_COMPILE);
	Draw_Scene();
	glEndList();
}

void getFPS()
{
	static int frame = 0, time, timebase = 0;
	static char buffer[256];

	char mode[64];
	sprintf_s<64>(mode, " (%.4f,%.4f,%.4f)", observer[0], observer[1], observer[2]);

	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		sprintf_s<256> (buffer, "FPS:%4.2f %s IPS:%4.2f",
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

void Draw_Table_List()
{
	glCallList(tableList);
}

void DrawCross(){
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);// 选择投影矩阵
	glPushMatrix();// 保存原矩阵
		glLoadIdentity();// 装入单位矩阵
		glOrtho(0, wWidth, 0, wHeight, -1, 1);// 位置正投影
		glMatrixMode(GL_MODELVIEW);// 选择Modelview矩阵
		glPushMatrix();// 保存原矩阵
			glLoadIdentity();// 装入单位矩阵*/
			glRasterPos2d(wWidth/2, wHeight/2);
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '+');
			glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void Draw_GUI(){
	DrawCross();
}

void redraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
	glLoadIdentity();									// Reset The Current Modelview Matrix

	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		0, 1, 0);				// 场景（0，0，0）的视点中心 (0,5,50)，Y轴向上

	if (bWire) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	glEnable(GL_DEPTH_TEST);

	Draw_Scene_Dynamic();
	if (world.changed) {
		RegenTableList(tableList);
		world.changed = false;
	}
	Draw_Table_List();

	getFPS();
	Draw_GUI();

	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, grey);
    GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_pos[] = { 4.5, 4.5, 0.5, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	int state = 7;
	glLightfv(GL_LIGHT0, GL_AMBIENT, (state&1)?white:black);
	glLightfv(GL_LIGHT0, GL_SPECULAR, (state&2)?white:black);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, (state&4)?white:black);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.03);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01);
	glEnable(GL_LIGHT0);

	glutSwapBuffers();
}

int main (int argc,  char *argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(1024,600);
	int windowHandle = glutCreateWindow("Simple MC");
	glewInit();

	render.init();
	tableList = GenTableList();
	keyboard.init();

	//glEnable(GL_CULL_FACE);
	init_cursor();
	glLineWidth(3.0);
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutSetCursor(GLUT_CURSOR_NONE);

	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
	if (glewIsSupported("GL_VERSION_2_0"))
		printf("Ready for OpenGL 2.0\n");
	else {
		printf("OpenGL 2.0 not supported\n");
		exit(1);
	}

#ifdef _ENABLE_CUSTOM_SHADERS_
	setShaders();
#endif

	glutMainLoop();
	return 0;
}