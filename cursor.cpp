#include "cursor.h"
#include "world.h"

extern World world;
extern block_and_face seen_block;
Cursor cursor;

void process_move(int x, int y){
	static int cX, cY, dx, dy;
	cX = glutGet(GLUT_WINDOW_WIDTH) >> 1;
	cY = glutGet(GLUT_WINDOW_HEIGHT) >> 1;
	dx = x - cX;
	dy = y - cY;
	if (dx*dx + dy*dy > 10) {
		glutWarpPointer(cX, cY);
		static flt len_x = 0.5, len_y = -0.25;
		cursor.h_ang += dx * len_x;
		cursor.h_ang %= 360;
		cursor.v_ang += dy * len_y;
		if (abs(cursor.v_ang) >= 90){
			cursor.v_ang = cursor.v_ang > 0 ? 89 : -89;
		}
		cursor.update();
		update_center();
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
			world.place_block(seen_block, DIRT);
		}
	}
}

void init_cursor(){
	glutMotionFunc(process_move);
	glutPassiveMotionFunc(process_move);
	glutMouseFunc(process_click);
}