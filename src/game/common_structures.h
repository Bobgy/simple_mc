#pragma once

#include "utility/vec.h"

struct MovementIntent {
	bool   is_on_ground = false;
	Vec2f  walk_intent = {0.0f, 0.0f};
	Radian yaw_intent = 0.0f;
	Radian pitch_intent = 0.0f;
	flt jump_intent = 0.0f;
	flt float_intent = 0.0f;
	bool isWalking(Radian current_yaw) const {
		return walk_intent || fabs(abs_delta_angle(yaw_intent, current_yaw) > 0.1f);
	}
	Vec2f getDesiredVelocity() const {
		Vec2f dir = yaw_intent.getHorizontalVector();
		return dir * walk_intent[0] + Vec2f{-dir[1], dir[0]} * walk_intent[1];
	}
};