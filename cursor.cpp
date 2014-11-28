#include "cursor.h"
#include "world.h"
#include "Render.h"

extern World world;
extern block_and_face seen_block;
Cursor cursor;
extern int windowHandle;
extern block_type type;
extern Render render;

void process_move(int x, int y){
	static int cX, cY, dx, dy;
	cX = glutGet(GLUT_WINDOW_WIDTH) >> 1;
	cY = glutGet(GLUT_WINDOW_HEIGHT) >> 1;
	dx = x - cX;
	dy = y - cY;
	if (abs(dx) + abs(dy) > 2 && glutGetWindow()==windowHandle) {
		glutWarpPointer(cX, cY);
		flt &ha = cursor.h_ang, &va = cursor.v_ang;
		const static flt len_x = 0.01, len_y = -0.006;
		ha += dx * len_x;
		if (ha > PI) ha -= 2 * PI;
		if (ha < -PI) ha += 2 * PI;
		va += dy * len_y;
		if (va + EPS > PI*0.5) va = PI*0.5 - EPS;
		if (va - EPS < PI*-0.5) va = -PI*0.5 + EPS;
		render.update_center(cursor);
		updateView();
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


void Cursor::update_facing_vector(){
	face = cursor.face;
	face_xz = cursor.face_xz;
	face_xz[0] = cos(h_ang);
	face_xz[2] = sin(h_ang);
	face_xz[1] = 0;
	face[0] = face_xz[0] * cos(v_ang);
	face[2] = face_xz[2] * cos(v_ang);
	face[1] = sin(v_ang);
}

void init_cursor(){
	glutMotionFunc(process_move);
	glutPassiveMotionFunc(process_move);
	glutMouseFunc(process_click);
}