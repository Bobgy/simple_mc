#ifndef _CURSOR_H
#define _CURSOR_H

#include <stdafx.h>

class ViewController;

const flt DEG2RAD = acos(-1.0f) / 180.0f;
const flt L_EPS = 1e-3f;
void init_cursor();

class ViewController {

private:

	// sensitivity for horizontal and vertical axes
	flt h_sen, v_sen;
	
	// rotation speed for horizontal and vertical axes
	flt h_rotation_speed, v_rotation_speed;

public:
	// constructor
	// h_sen: horizontal sensitivity
	// v_sen: vertical sensitivity
	ViewController(flt h_sen = 1.0f, flt v_sen = 1.0f):
		h_sen(h_sen), v_sen(v_sen),
		h_rotation_speed(0.0f), v_rotation_speed(0.0f)
	{
		// do nothing
	}

	void setup(flt _h_sen, flt _v_sen) {
		h_sen = _h_sen;
		v_sen = _v_sen;
	}

	// get rotation speeds
	// return: pair<h_rotation_speed, v_rotation_speed>
	pair<flt, flt> getViewRotationSpeed() {
		return make_pair(h_rotation_speed, v_rotation_speed);
	}

	void handle_cursor_move(int dx, int dy);
	
	void tick(flt delta_time);

	bool need_update() const {
		return h_rotation_speed || v_rotation_speed;
	}
};

#endif