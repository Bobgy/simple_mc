#ifndef _CURSOR_H
#define _CURSOR_H

#include "stdafx.h"

extern void update_center();
extern void updateView();

const flt DEG2RAD = acos(-1.0) / 180.0;
const flt L_EPS = 1e-3f;
void init_cursor();

class Cursor{

private:

	flt h_ang, v_ang;

public:

	//The vector denoting the facing direction and facing direction in the xz plane
	Vec3f face, face_xz;

	void update_facing_vector();

	flt get_horizontal_angle() const {
		return h_ang;
	}
	flt get_vertical_angle() const {
		return v_ang;
	}
	Cursor() :h_ang(0), v_ang(0) {}
	friend void process_move(int, int);

};

#endif