#include "view.h"
#include "world.h"
#include "Render.h"

extern World world;
extern BlockAndFace seen_block;
View main_view;
extern int windowHandle;
extern block_type type;
extern Render render;

void process_move(int x, int y){
	static int cX, cY, dx, dy;
	cX = glutGet(GLUT_WINDOW_WIDTH) >> 1;
	cY = glutGet(GLUT_WINDOW_HEIGHT) >> 1;
	dx = x - cX;
	dy = y - cY;
	if (abs(dx) + abs(dy) > 0 && glutGetWindow() == windowHandle) {
		glutWarpPointer(cX, cY);
		main_view.handle_cursor_move(dx, dy);
	}
}

void process_click(int button, int state, int x, int y){
	if (seen_block.second == -1)return;
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN){
			world.destroy_block(seen_block.first);
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN){
			world.place_block(seen_block, type);
		}
	}
}


void View::update_facing_vector(){
	face_xz[0] = cos(h_ang);
	face_xz[2] = sin(h_ang);
	face_xz[1] = 0;
	face[0] = face_xz[0] * cos(v_ang);
	face[2] = face_xz[2] * cos(v_ang);
	face[1] = sin(v_ang);
}

void View::clamp_angles()
{
	if (h_ang > PI) h_ang -= 2 * PI;
	if (h_ang < -PI) h_ang += 2 * PI;
	if (v_ang + L_EPS > HALF_PI) v_ang = HALF_PI - L_EPS;
	if (v_ang - L_EPS < -HALF_PI) v_ang = -HALF_PI + L_EPS;
}

void View::handle_cursor_move(int dx, int dy)
{
	h_rotation_speed += (flt)dx * h_sen;
	v_rotation_speed += (flt)dy * v_sen;
}


void View::tick(flt delta_time)
{
	if (h_rotation_speed != 0.0f || v_rotation_speed != 0.0f)
	{
		h_ang += h_rotation_speed;
		v_ang += v_rotation_speed;
		m_need_update = true;
	}
	clamp_angles();
	h_rotation_speed = h_rotation_speed / 2.0;
	h_rotation_speed = sgn(h_rotation_speed) ? h_rotation_speed : 0.0f;
	v_rotation_speed = v_rotation_speed / 2.0;
	v_rotation_speed = sgn(v_rotation_speed) ? v_rotation_speed : 0.0f;
}

void init_cursor() {
	glutMotionFunc(process_move);
	glutPassiveMotionFunc(process_move);
	glutMouseFunc(process_click);
}

void tick_view(flt delta_time)
{
	main_view.tick(delta_time);
	if (main_view.need_update()) {
		render.update_center(main_view);
	}
}
