#include "stdafx.h"

#include "utility/view.h"

#include "game/game.h"
#include "game/world.h"
#include "game/block.h"
#include "render/render.h"


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
	m_rotation_speed += (m_rotation_sen | Vec2f{(flt)dx, (flt)dy});
}


void ViewController::tick(flt delta_time)
{
	m_rotation_speed[0] /= 2.0f;
	m_rotation_speed[0] = sgn(m_rotation_speed[0]) ? m_rotation_speed[0] : 0.0f;
	m_rotation_speed[1] /= 2.0f;
	m_rotation_speed[1] = sgn(m_rotation_speed[1]) ? m_rotation_speed[1] : 0.0f;
}

void init_cursor()
{
	glutMotionFunc(process_move);
	glutPassiveMotionFunc(process_move);
	glutMouseFunc(process_click);
}
