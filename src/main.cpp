#define _CRT_SECURE_NO_WARNINGS
#define CULL_BACK

#include "stdafx.h"

#include <math.h>
#include <map>
#include <ctime>
#include <cassert>
#include <iostream>
#include <list>

#include <core/world.h>
#include <core/entity.h>
#include <core/item.h>
#include <core/block.h>
#include <core/view.h>

#include <render/render.h>
#include <render/shader.h>
#include <render/auxiliary.h>
#include <render/shadow.h>

#include <utility/keyboard.h>
#include <utility/lodepng.h>
#include <utility/obj.h>


using namespace std;

//The light's position
flt light_pos[] = { 1.9f, 1.0f, -2.0f, 0.0f };

block_type type;

int wHeight = 724;
int wWidth = 1024;
extern GLuint tex;
GLhandleARB shader_id;
int screenshot_count = 0;

typedef Vec3i Pt3;
//World world(time(NULL), 30);
World world("stage/last_save.txt");

BlockAndFace seen_block = make_pair(Vec3i(), -1);
extern Render render;
extern Keyboard keyboard;
extern View main_view;

flt pp[3] = { 5.0f, 3.0f, 0.0f }, vv[3] = { 0.0f, 0.0f, 0.0f };
Entity observer(pp, vv, r, h, 1.0);
float deg2rad = PI / 180.0;
int windowHandle;

extern bool bObserver;
list<Item> items;

/*
	This function is temporarily empty.
	And it will be used to load the information of each obj-item we need to draw.
*/
void load_obj()
{
	//items.push_back(Item(Vec3i(3, 2, 5), "objData/eagle.obj", 1, 0.5, Vec3f(0, 0, 0)));
	//items.push_back(Item(Vec3i(8, 2, 5), "objData/soccerball.obj", 2, 0.1, Vec3f(0, 0, 0)));
	//items.push_back(Item(Vec3i(16, 2, 10), "objData/castle.obj", 5, 0.13, Vec3f(0, 0, 0)));
	//items.push_back(Item(Vec3i(3, 2, 10), "objData/rose+vase.obj", 2, 0.005, Vec3f(0, 0, 0)));
}

void reshape(int width, int height)
{
	if (height == 0) height = 1;
	wHeight = height;
	wWidth = width;
}

float dis = 1.0;

flt step = 0.3, eps = 1e-8;
int idle_count = 0;

void tick_main(flt delta_time)
{
	static flt saved_time = 0.0f;
	saved_time += delta_time;
	flt frame_interval = 1.0f / FRAME_RATE;
	if (saved_time >= frame_interval) {
		saved_time -= frame_interval;
		if (saved_time > frame_interval * 3.0f) {
			saved_time = 0.0f;
		}
		tick_view(frame_interval);

		if (bGravity) observer.fall();
		int df = 0;
		static flt ff[3];
		if (keyboard.get_state('w') ^ keyboard.get_state('s')) {
			df = keyboard.get_state('w') ? 1 : -1;
			observer.give_velocity(main_view.face_xz, step*df);
		}
		if (keyboard.get_state('a') ^ keyboard.get_state('d')) {
			df = keyboard.get_state('a') ? 1 : -1;
			ff[0] = main_view.face_xz[2];
			ff[1] = 0;
			ff[2] = -main_view.face_xz[0];
			observer.give_velocity(ff, step*0.5*df);
		}
		if (bGravity) {
			if (keyboard.get_state(' ') && observer.on_ground) {
				observer.force(Vec3f(0, 13, 0));
			}
		}
		else {
			if (keyboard.get_state(' '))
				observer.give_velocity(Vec3f(0, 0.5, 0), 1);
			if (keyboard.get_special_state(GLUT_KEY_SHIFT_L))
				observer.give_velocity(Vec3f(0, -0.5, 0), 1);
		}
		static int p[3];
		for (int i = 0; i < 3; ++i)
			p[i] = floor(observer[i]);
		p[1] = floor(observer[1] + 0.5*h);
		observer.on_ground = false;
		for (int dx = -1; dx <= 1; ++dx) {
			for (int dy = -2; dy <= 2; ++dy) {
				for (int dz = -1; dz <= 1; ++dz) {
					static map<Pt3, Block*>::const_iterator it;
					if ((it = world.find(Pt3({ p[0] + dx, p[1] + dy, p[2] + dz }))) != world.end()) {
						observer.on_ground |= observer.collide_cube_vertically(it->first);
						observer.collide_cube_horizontally(it->first);
					}
				}
			}
		}
		/*
		Those codes are used to detect whether the observer touched an obj-item.
		If it is, that item is got by the observer.
		*/
		for (auto &x : items) {
			if (x.draw) {
				Vec3i temp = Pt3(p) - x.loc;
				int r = temp * temp;
				if (r <= range_touch)
				{
					x.set_false();
					world.add(x.type);
				}
			}
		}
		/*
		mark for the end
		*/
		if (observer.on_ground) observer.be_slowed(smoothness_ground);
		observer.update();
		render.update_center(main_view);
		
		saved_time = 0.0f;
	}
}

void idle()
{
	// game tick
	static clock_t lst = 0;
	clock_t now = clock();
	tick_main(flt(now - lst) / (flt)CLOCKS_PER_SEC);
	lst = now;

	++idle_count;

	static flt ang = 1.5, lst_time = 0;
	if (bMovingLight) {
		flt time = glutGet(GLUT_ELAPSED_TIME) / 2000.0;
		ang += time - lst_time;
		lst_time = time;
		light_pos[0] = 1.0;
		light_pos[1] = sin(ang);
		light_pos[2] = cos(ang);
	}
	else lst_time = glutGet(GLUT_ELAPSED_TIME) / 2000.0;

	glutPostRedisplay();
}

extern int tableList;

void init(int argc, char *argv[]) {
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
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	glLineWidth(3.0);
	glutSetCursor(GLUT_CURSOR_NONE);

	shader_id = getShaders();

	load_obj();

	tableList = genTableList();
}


int main(int argc, char *argv[])
{
	init(argc, argv);
	glutMainLoop();
	return 0;
}
