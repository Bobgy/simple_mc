// This assignment may cost you some efferts, so I give you some important HINTS, hope that may help you.
// Enjoy the coding and thinking, do pay more attention to the library functions used in OPENGL, such as how they are used? what are the parameters used? and why?

// 实验报告里面多写点感想，即对函数的理解，以及调用的顺序，步骤等。思考为什么是这样子调用的，为什么参数是这样子设置的？。。。等等你觉得值得研究的问题。
#include <stdlib.h>
#include <math.h>
#include "glut.h"
#include <map>
#include <array>
using namespace std;

float fTranslate;
float fRotate;
float fScale     = 1.0f;	// set inital scale value to 1.0f

bool bPersp = true;
bool bAnim = false;
bool bWire = false;

int wHeight = 500;
int wWidth = 500;

//todo
//hint: some additional parameters may needed here when you operate the teapot

void Draw_Leg()
{
	glScalef(1, 1, 3);
	glutSolidCube(1.0);
}

float pot_x=0, pot_y=0, ang=0;
bool pot_rotate=0;

typedef array<int, 3> Pt3;
map<Pt3, int> block;
void init(){
	for (int i = -16; i <= 16; ++i)
		for (int j = -16; j <= 16; ++j)
			for (int k = 0; k <= 2; ++k){
				int t = (1 << (k + 2)) - 1;
				if (k == 0 || (rand()&t)==t) block[Pt3({ i, k, j })] = 1;
				else break;
			}
}
void Draw_Scene()
{
	glPushMatrix();
	glTranslatef(pot_x, pot_y, 4+1);
	glRotatef(90, 1, 0, 0); //notice the rotation here, you may have a TRY removing this line to see what it looks like.
	if(pot_rotate)glRotatef(ang, 0, 1, 0);
	//todo; hint: when operate the teapot, you may need to change some parameters
	for (auto x : block){
		glPushMatrix();
			const Pt3 &p = x.first;
			glScalef(3.0, 3.0, 3.0);
			glTranslatef(p[0], p[1]-4.0, p[2]);
			glutSolidCube(1.0);
			glColor3f(0.0, 0.0, 0.0);
			glutWireCube(1.0001);
		glPopMatrix();
	}

	glutSolidTeapot(1);
	glPopMatrix();
	/*
	glPushMatrix();
	glTranslatef(0, 0, -2);
	glScalef(100, 100, 0.2);
	glutSolidCube(1.0);
	glPopMatrix();
	*/
	glPushMatrix();
	glTranslatef(0, 0, 3.5);
	glScalef(5, 4, 1);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.5, 1, 1.5);
	Draw_Leg();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.5, 1, 1.5);
	Draw_Leg();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(1.5, -1, 1.5);
	Draw_Leg();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-1.5, -1, 1.5);
	Draw_Leg();
	glPopMatrix();

}

float kk = 0;

void updateView(int width, int height)
{
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	float whRatio = (GLfloat)width/(GLfloat)height;
	
	if (bPersp) gluPerspective(40*exp(kk), whRatio, 0.1, 300);
	else glOrtho(-3 ,3, -3, 3,-100,100);
	glViewport(0, 0, width, height);					// Reset The Current Viewport

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

void reshape(int width, int height)
{
	if (height==0)height=1;

	wHeight = height;
	wWidth = width;

	updateView(wWidth, wHeight);
}

void idle()
{
	glutPostRedisplay();
}

float center[] = {0, 0, 0};
float eye[] = {0, 0, 8};
float PI = acos(-1.0), deg2rad = PI / 180.0;
int horizontal_angle = 0, verticle_angle = 0;
float face[] = { 1, 0, 0 };
void update_dir(){
	float ha = horizontal_angle * deg2rad,
		  va = verticle_angle * deg2rad;
	face[0] = cos(ha) * cos(va);
	face[2] = sin(ha) * cos(va);
	face[1] = sin(va);
}
void update_center(){
	update_dir();
	for (int i = 0; i < 3; ++i)
		center[i] = eye[i] + face[i];
}
void process_move(int x, int y){
	static int cX, cY, dx, dy;
	cX = glutGet(GLUT_WINDOW_WIDTH) >> 1;
	cY = glutGet(GLUT_WINDOW_HEIGHT) >> 1;
	dx = x - cX;
	dy = y - cY;
	if (dx*dx + dy*dy > 10) {
		glutWarpPointer(cX, cY);
		static float len_x = 0.5, len_y = -0.25;
		horizontal_angle += dx * len_x;
		horizontal_angle = (horizontal_angle + 360) % 360;
		verticle_angle = verticle_angle + dy * len_y;
		if (verticle_angle >= 90) verticle_angle = 89;
		if (verticle_angle <= -90) verticle_angle = -89;
		update_center();
	}
}

float step=0.2, eps=1e-8;
void chg(float &x, float delta, float max){
	if(fabs(x+delta)<max+eps)x+=delta;
}
void key(unsigned char k, int x, int y)
{
	switch(k)
	{
	case 27:
	case 'q': {exit(0); break; }
	case 'p': {bPersp = !bPersp; updateView(wWidth, wHeight);break; }

	case ' ': {bAnim = !bAnim; break;}
	case 'o': {bWire = !bWire; break;}

	case 'a':
	case 'd':
	case 'w':
	case 's':{
		int df = 0;
		if (k == 'w' || k == 's'){
			df = k == 'w' ? 1 : -1;
			for (int i = 0; i < 3; ++i)
				eye[i] += face[i] * df * step;
		} else {
			df = k == 'a' ? 1 : -1;
			float len = sqrt(face[2] * face[2] + face[0] * face[0]);
			eye[0] += face[2] / len * df * step;
			eye[2] -= face[0] / len * df * step;
		}
		update_center();
		break;
	}
	case 'z': {//todo
		chg(kk, 0.1, 2);
		updateView(wWidth, wHeight);
		break;
			  }
	case 'c': {//todo
		chg(kk, -0.1, 2);
		updateView(wWidth, wHeight);
		break;
			  }

  //茶壶相关操作
	case 'l': {//todo, hint:use the ARRAY that you defined, and notice the teapot can NOT be moved out the range of the table.
		chg(pot_x,-0.1,1);
		break;
			  }
	case 'j': {//todo
		chg(pot_x,0.1,1);
		break;
			  }
	case 'i': {//todo
		chg(pot_y,0.1,1);
		break;
			  }
	case 'k': {//todo
		chg(pot_y,-0.1,1);
		break;
			  }
	case 'e': {//todo
		pot_rotate^=1;
		break;
			  }
	}
	update_center();
}


void redraw()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	glEnable(GL_LIGHTING);
    GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_pos[] = {5,5,5,1};
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glEnable(GL_LIGHT0);
	
//	glTranslatef(0.0f, 0.0f,-6.0f);			// Place the triangle at eye
	glRotatef(fRotate, 0, 1.0f, 0);			// Rotate around Y axis
	glRotatef(-90, 1, 0, 0);
	glScalef(0.2, 0.2, 0.2);
	Draw_Scene();						// Draw Scene

	if (bAnim) fRotate  += 0.5f;
	if (pot_rotate) ang += 4.0f;
	//todo; hint: when you want to rotate the teapot, you may like to add another line here =)
	glutSwapBuffers();
}

int main (int argc,  char *argv[])
{
	init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(1024,600);
	int windowHandle = glutCreateWindow("Simple MC");

	glLineWidth(3.0);
	glEnable(GL_LINE_SMOOTH);
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);	
	glutKeyboardFunc(key);
	glutIdleFunc(idle);
	glutMotionFunc(process_move);
	glutPassiveMotionFunc(process_move);
	glutSetCursor(GLUT_CURSOR_NONE);

	glutMainLoop();
	return 0;
}