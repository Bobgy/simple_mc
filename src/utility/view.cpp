#include "render/render.h"
#include "game/game.h"
#include "utility/view.h"
#include "game/block.h"

extern BlockAndFace seen_block;
extern int windowHandle;
extern block_type type;

void process_move(int x, int y)
{
	static int cX, cY, dx, dy;
	cX = glutGet(GLUT_WINDOW_WIDTH) >> 1;
	cY = glutGet(GLUT_WINDOW_HEIGHT) >> 1;
	dx = x - cX;
	dy = y - cY;
	if ((dx || dy) && glutGetWindow() == windowHandle) {
		glutWarpPointer(cX, cY);
		ViewController *view_controller = CurrentGame()->getViewController();
		view_controller->handle_cursor_move(dx, dy);
	}
}

void process_click(int button, int state, int x, int y)
{
	if (seen_block.second == -1) return;
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			CurrentGame()->getWorld()->destroy_block(seen_block.first);
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			CurrentGame()->getWorld()->place_block(seen_block, type);
		}
	}
}

void ViewController::handle_cursor_move(int dx, int dy)
{
	h_rotation_speed += (flt)dx * h_sen;
	v_rotation_speed += (flt)dy * v_sen;
}


void ViewController::tick(flt delta_time)
{
	h_rotation_speed = h_rotation_speed / 2.0;
	h_rotation_speed = sgn(h_rotation_speed) ? h_rotation_speed : 0.0f;
	v_rotation_speed = v_rotation_speed / 2.0;
	v_rotation_speed = sgn(v_rotation_speed) ? v_rotation_speed : 0.0f;
}

void init_cursor()
{
	glutMotionFunc(process_move);
	glutPassiveMotionFunc(process_move);
	glutMouseFunc(process_click);
}
