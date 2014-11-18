#include <stdlib.h>
#include <math.h>
#include <map>
#include <array>
#include <ctime>
#include <cassert>
#include "vec.h"
#include "world.h"
#include "glut.h"
#include "entity.cpp"
using namespace std;

float fTranslate;
float fRotate;
float fScale     = 1.0f;	// set inital scale value to 1.0f

bool bPersp = true;
bool bAnim = false;
bool bWire = false;

int wHeight = 500;
int wWidth = 500;

float pot_x = 0, pot_y = 0, ang = 0, r = 0.45, h = 1.6;
bool pot_rotate=0;

//typedef array<int, 3> Pt3;
typedef Vec3i Pt3;
map<Pt3, int> block;
flt pp[3] = { 0, 10, 0 }, vv[3] = { 0, 0, 0 };
entity::Entity observer(pp, vv, r, h, 1.0);
void init(){
	srand(time(0));
	for (int i = -32; i <= 32; ++i)
		for (int j = -32; j <= 32; ++j)
			for (int k = 0; k <= 3; ++k){
				int t = (1 << (k + 2)) - 1;
				if (k == 0 || (rand()&t) == t) block[Pt3({ i, k, j })] = 1;
				else break;
			}
}
bool enableObserver = 0;
void DrawObserver(){
	if (!enableObserver) return;
	GLUquadricObj *objCylinder = gluNewQuadric();
	glPushMatrix();
	glTranslatef(observer[0], observer[1]+h, observer[2]);
	glRotatef(90, 1, 0, 0);
	gluCylinder(objCylinder, r, r, 1, 30, 10);
	//glutWireCube(1.0);
	glPopMatrix();
}
void Draw_Scene()
{
	glPushMatrix();
	glScalef(1.0, 1.0, 1.0);
	for (auto x : block){
		glPushMatrix();
		const Pt3 &p = x.first;
		glTranslatef(p[0]+0.5, p[1]+0.5, p[2]+0.5);
		glutSolidCube(1.0);
		//glutWireCube(1.01);
		glPopMatrix();
	}
	DrawObserver();
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
	if (height==0) height=1;

	wHeight = height;
	wWidth = width;

	updateView(wWidth, wHeight);
}

float center[] = {0, 4, 0};
float eye[] = {0, 4, 8};
float PI = acos(-1.0), deg2rad = PI / 180.0;
int horizontal_angle = 0, verticle_angle = 0;
flt face[] = { 1, 0, 0 };
flt face_xz[] = { 1, 0, 0 };
void update_dir(){
	float ha = horizontal_angle * deg2rad,
		  va = verticle_angle * deg2rad;
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
		center[i] = observer[i] + (i == 1 ? h*0.8 : 0);
		eye[i] = center[i] + face[i] * dis;
		swap(center[i], eye[i]);
	}
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
		updateView(wWidth, wHeight);
	}
}

int idle_count = 0;
clock_t lst = 0, inter = entity::CLOCK_T * CLOCKS_PER_SEC;
void idle()
{
	++idle_count;
	static clock_t now;
	now = clock();
	if (lst + inter <= now) {
		observer.fall();
		static int p[3];
		for (int i = 0; i < 3; ++i)
			p[i] = floor(observer[i]);
		p[1] = floor(observer[1] - 0.5*h);
		for (int dx = -1; dx <= 1; ++dx){
			for (int dy = -2; dy <= 2; ++dy){
				for (int dz = -1; dz <= 1; ++dz){
					static map<Pt3, int>::iterator it;
					if ((it = block.find(Pt3({ p[0] + dx, p[1] + dy, p[2] + dz }))) != block.end()){
						observer.collide_cube_horizontally(&it->first);
						observer.collide_cube_vertically(&it->first);
					}
				}
			}
		}
		observer.update();
		update_center();
		updateView(wWidth, wHeight);
		lst += inter;
	}
	glutPostRedisplay();
}

flt step=0.3, eps=1e-8;
void chg(float &x, float delta, float max){
	if(fabs(x+delta)<max+eps)x+=delta;
}
void key(unsigned char k, int x, int y)
{
	switch(k)
	{
	case 27:
	case 'q': {exit(0); break; }
	case 'p': {bPersp = !bPersp; break; }

	case 'o': {bWire = !bWire; break;}

	case 'a':
	case 'd':
	case 'w':
	case 's':{
		int df = 0;
		static flt ff[3];
		if (k == 'w' || k == 's'){
			df = k == 'w' ? 1 : -1;
			observer.give_velocity(face_xz, step*df);
		} else {
			df = k == 'a' ? 1 : -1;
			ff[0] = face_xz[2];
			ff[1] = 0;
			ff[2] = -face_xz[0];
			observer.give_velocity(ff, step*df);
		}
		break;
	}
	case ' ':{
		static flt ff[3];
		for (int i = 0; i < 3; ++i)ff[i] = 0;
		ff[1] = 15;
		observer.force(ff);
		break;
	}
	case 'z': {
		chg(kk, 0.1, 2);
		break;
			  }
	case 'c': {
		chg(kk, -0.1, 2);
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
	updateView(wWidth, wHeight);
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

GLint tableList;

GLint GenTableList()
{
	GLint lid = glGenLists(1);
	glNewList(lid, GL_COMPILE);
	Draw_Scene(); //Call Draw_table() to draw the table and bunnies into the list
	glEndList();
	return lid;
}

void Draw_Table_List()
{
	glCallList(tableList);
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
	//glEnable(GL_COLOR_MATERIAL);
	//glEnable(GL_LINE_SMOOTH);

    GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_pos[] = {5,5,5,1};
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glEnable(GL_LIGHT0);
	
	Draw_Table_List();
	//Draw_Scene();						// Draw Scene

	getFPS();

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

	tableList = GenTableList();

	glLineWidth(3.0);
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