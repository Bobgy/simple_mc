#pragma once

#include "utility/vec.h"

struct MovementIntent {
	bool is_on_ground = false;
	Vec2f walk_intent = {0.0f, 0.0f};
	flt yaw_intent = 0.0f;
	flt pitch_intent = 0.0f;
	flt jump_intent = 0.0f;
	flt float_intent = 0.0f;
	bool isWalking(flt current_yaw) const {
		return walk_intent || fabs(abs_delta_angle(yaw_intent, current_yaw) > 0.1f);
	}
};