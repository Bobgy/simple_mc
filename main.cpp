#define _CRT_SECURE_NO_WARNINGS
#define _ENABLE_CUSTOM_SHADERS_
//#define _MOVING_LIGHT_
#define CULL_BACK

#include "stdafx.h"

#include <math.h>
#include <map>
#include <ctime>
#include <cassert>
#include <iostream>
#include "world.h"
#include "entity.h"
#include "render.h"
#include "keyboard.h"
#include "cursor.h"
#include "shader.h"
#include "auxiliary.h"
#include "shadow.h"
#include "lodepng.h"
#include "config.h"

using namespace std;

//The light's position
GLfloat light_pos[] = { 1.9, 1.0, 0.5, 0 };
block_type int2block_type[10] = { TREASURE, AIR, DIRT, RED, BLUE, YELLOW, GREEN, STONE, GLASS, WOOD };
float fTranslate;
float fRotate;
float fScale     = 1.0f;	// set inital scale value to 1.0f

bool bPersp = true;
bool bGravity = true;
block_type type;

int wHeight =  724;
int wWidth = 1024;
extern GLuint tex;
GLhandleARB shader_id;
int screenshot_count = 0;

typedef Vec3i Pt3;
//World world(time(NULL), 30);
World world("stage/1.txt");

block_and_face seen_block = make_pair(Vec3i(), -1);
extern Render render;
extern KeyboardControl keyboard;
extern Cursor cursor;

flt pp[3] = { 0, 10, 0 }, vv[3] = { 0, 0, 0 };
Entity observer(pp, vv, r, h, 1.0);
float deg2rad = PI / 180.0;
int windowHandle;

extern bool bObserver;

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

float dis = 1.0;

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
		if (keyboard.get_state('w') ^ keyboard.get_state('s')){
			df = keyboard.get_state('w') ? 1 : -1;
			observer.give_velocity(cursor.face_xz, step*df);
		}
		if (keyboard.get_state('a') ^ keyboard.get_state('d')){
			df = keyboard.get_state('a') ? 1 : -1;
			ff[0] = cursor.face_xz[2];
			ff[1] = 0;
			ff[2] = -cursor.face_xz[0];
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
		p[1] = floor(observer[1] + 0.5*h);
		observer.on_ground = false;
		for (int dx = -1; dx <= 1; ++dx){
			for (int dy = -2; dy <= 2; ++dy){
				for (int dz = -1; dz <= 1; ++dz){
					static map<Pt3, Block*>::const_iterator it;
					if ((it = world.find(Pt3({ p[0] + dx, p[1] + dy, p[2] + dz }))) != world.end()){
						observer.on_ground |= observer.collide_cube_vertically(it->first);
						observer.collide_cube_horizontally(it->first);
					}
				}
			}
		}
		if (observer.on_ground) observer.be_slowed(smoothness_ground);
		observer.update();
		render.update_center(cursor);
		updateView();
		lst += inter;
	}
	glutPostRedisplay();
}


extern int tableList;

void init(int argc, char *argv[]){
	type = DIRT;
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
