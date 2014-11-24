#ifndef _CURSOR_H
#define _CURSOR_H

#include "vec.h"
#include "glut.h"

extern void update_center();
extern void updateView();

const flt DEG2RAD = acos(-1.0) / 180.0;
void init_cursor();

class Cursor{

private:

	int h_ang, v_ang;
	flt h_ang_f, v_ang_f;

	void update(){
		h_ang_f = h_ang * DEG2RAD;
		v_ang_f = v_ang * DEG2RAD;
	}

public:

	flt get_horizontal_angle() const {
		return h_ang_f;
	}
	flt get_vertical_angle() const {
		return v_ang_f;
	}
	Cursor() :h_ang(0), v_ang(0) {
		update();
	}
	friend void process_move(int, int);

};

#endif