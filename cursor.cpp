#include "cursor.h"

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