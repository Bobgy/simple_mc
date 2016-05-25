#ifndef _CURSOR_H
#define _CURSOR_H

#include <stdafx.h>

class ViewController;

const flt DEG2RAD = acos(-1.0f) / 180.0f;
const flt L_EPS = 1e-3f;
void init_cursor();

class ViewController {

protected:

	// sensitivity for horizontal and vertical axes
	flt h_sen, v_sen;
	flt h_rot, v_rot;
	Vec2f m_rotation_sen = {1.0f, 1.0f};
	
	// rotation speed for horizontal and vertical axes
	Vec2f m_rotation_speed = {0.0f, 0.0f};

public:
	// constructor
	// h_sen: horizontal sensitivity
	// v_sen: vertical sensitivity
	ViewController(flt h_sen = 1.0f, flt v_sen = 1.0f):
		m_rotation_sen{h_sen, v_sen}
	{
		// do nothing
	}

	void setup(flt h_sen, flt v_sen) {
		m_rotation_sen = {h_sen, v_sen};
	}

	// get rotation speeds
	// return: pair<h_rotation_speed, v_rotation_speed>
	Vec2f getViewRotationSpeed() {
		return m_rotation_speed;
	}

	void handle_cursor_move(int dx, int dy);
	
	void tick(flt delta_time);

	bool need_update() const {
		return bool(m_rotation_speed);
	}
};

#endif