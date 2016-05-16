#ifndef _CURSOR_H
#define _CURSOR_H

#include <stdafx.h>

extern void update_center();
extern void updateView();

const flt DEG2RAD = acos(-1.0) / 180.0;
const flt L_EPS = 1e-3f;
void init_cursor();
void tick_view(flt delta_time);

class View {

private:

	// current horizontal angle and vertical angle
	flt h_ang, v_ang;

	// sensitivity for horizontal and vertical axes
	flt h_sen, v_sen;
	
	// rotation speed for horizontal and vertical axes
	flt h_rotation_speed, v_rotation_speed;

	bool m_need_update;

public:

	//The vector denoting the facing direction and facing direction in the xz plane
	Vec3f face, face_xz;

	void update_facing_vector();

	void clamp_angles();

	flt get_horizontal_angle() const {
		return h_ang;
	}
	flt get_vertical_angle() const {
		return v_ang;
	}
	View(flt h_sen = 0.002f, flt v_sen = -0.001f):
		h_ang(0.0f), v_ang(0.0f),
		h_sen(h_sen), v_sen(v_sen),
		h_rotation_speed(0.0f), v_rotation_speed(0.0f),
		m_need_update(false)
	{
		// do nothing
	}

	void handle_cursor_move(int dx, int dy);
	
	void tick(flt delta_time);

	bool need_update() const {
		return m_need_update;
	}
};

#endif