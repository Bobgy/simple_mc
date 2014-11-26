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
#include "shader.h"
#include "auxiliary.h"

using namespace std;

float lpos[4] = { 1, 0.5, 1, 0 };

float fTranslate;
float fRotate;
float fScale     = 1.0f;	// set inital scale value to 1.0f

bool bPersp = true;
bool bAnim = false;
bool bWire = false;

int wHeight = 500;
int wWidth = 500;
extern GLuint tex;

float r = 0.45, h = 1.6;
typedef Vec3i Pt3;
World world(time(NULL));
block_and_face seen_block = make_pair(Vec3i(), -1);
extern Render render;
extern KeyboardControl keyboard;
extern Cursor cursor;

flt pp[3] = { 0, 10, 0 }, vv[3] = { 0, 0, 0 };
Entity observer(pp, vv, r, h, 1.0);
float center[] = { 0, 4, 0 };
float eye[] = { 0, 4, 8 };
float PI = acos(-1.0), deg2rad = PI / 180.0;
flt face[] = { 1, 0, 0 };
flt face_xz[] = { 1, 0, 0 };
int windowHandle;

bool enableObserver = 0;

void Draw_Scene_Dynamic(){
	if (enableObserver)
		DrawObserver(observer, r, h);
	DrawSeenBlock(seen_block);
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

extern int tableList;

void redraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
	glLoadIdentity();									// Reset The Current Modelview Matrix

	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		0, 1, 0);				// ������0��0��0�����ӵ����� (0,5,50)��Y������

	if (bWire) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	glEnable(GL_DEPTH_TEST);

	Draw_Scene_Dynamic();
	if (world.changed) {
		regenTableList(tableList);
		world.changed = false;
	}
	drawTableList();

	drawGUI(observer);

	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, grey);
	static const GLfloat light_pos[] = { 4.5, 4.5, 0.5, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	int state = 7;
	glLightfv(GL_LIGHT0, GL_AMBIENT, (state&1)?dark_grey:black);
	glLightfv(GL_LIGHT0, GL_SPECULAR, (state&2)?sun:black);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, (state&4)?sun:black);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.00);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00);
	glEnable(GL_LIGHT0);

	glutSwapBuffers();
}

int main (int argc,  char *argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(1024,600);
	windowHandle = glutCreateWindow("Simple MC");
	glewInit();

	render.init();
	tableList = genTableList();
	keyboard.init();

	glEnable(GL_CULL_FACE);
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