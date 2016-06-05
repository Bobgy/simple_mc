//#define CULL_BACK

#include "stdafx.h"

#include <cmath>
#include <map>
#include <ctime>
#include <cassert>
#include <iostream>
#include <list>

#include "game/game.h"
#include "game/world.h"
#include "game/entity.h"
#include "game/item.h"
#include "game/block.h"

#include <render/render.h>
#include <render/shader.h>
#include <render/auxiliary.h>
#include <render/shadow.h>

#include "utility/view.h"
#include <utility/keyboard.h>
#include <utility/lodepng.h>
#include <utility/obj.h>

#include "scripts/game/sg001.h"


using namespace std;

//The light's position
extern Vec3f light_pos;

block_type type;

int wHeight = 724;
int wWidth = 1024;
extern GLuint tex;
GLhandleARB shader_id;
int screenshot_count = 0;

BlockAndFace seen_block = make_pair(Vec3i(), -1);
extern Render render;

flt pp[3] = { 5.0f, 3.0f, 0.0f }, vv[3] = { 0.0f, 0.0f, 0.0f };
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
int idle_count = 0;

void tick_main(flt delta_time)
{
	World *world = CurrentGame()->getWorld();
	const Keyboard &keyboard = *CurrentGame()->getKeyboard();
	ViewController *view_controller = CurrentGame()->getViewController();
	static flt saved_time = 0.0f;
	saved_time += delta_time * GAME_SPEED;
	flt frame_interval = 1.0f / FRAME_RATE;
	if (saved_time >= frame_interval) {
		saved_time -= frame_interval;
		if (saved_time > frame_interval * 3.0f) {
			saved_time = 0.0f;
		}
		CurrentGame()->tick(frame_interval);
		render.update_center();
		
		/*
		Those codes are used to detect whether the observer touched an obj-item.
		If it is, that item is got by the observer.
		*/
		for (auto &x : items) {
			if (x.draw) {
				Vec3i temp = CurrentGame()->getPlayerEntity()->get_pos() - x.loc;
				int r = temp * temp;
				if (r <= range_touch)
				{
					x.set_false();
					world->add(x.type);
				}
			}
		}
		/*
		mark for the end
		*/

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
	
	// setup the game using sg001 script
	auto sg001 = make_shared<scripts::SG001>();
	sg001->setup();
	CurrentGame()->setup(sg001);
	
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
	LOG_INFO(__FUNCTION__, "Initializing...\n");
	init(argc, argv);
	LOG_INFO(__FUNCTION__, "Initialized\n");

	glutMainLoop();
	return 0;
}
