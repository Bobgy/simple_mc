#pragma once

#include "utility/vec.h"

class Entity;

class EntityController
{
// public typedef
public:
	struct MovementIntent {
		bool is_on_ground = false;
		Vec2f walk_intent = { 0.0f, 0.0f };
		flt yaw_intent = 0.0f;
		flt pitch_intent = 0.0f;
		flt jump_intent = 0.0f;
		flt float_intent = 0.0f;
		bool isWalking(flt current_yaw) const {
			return walk_intent || fabs(abs_delta_angle(yaw_intent, current_yaw) > 0.1f);
		}
	};

// protected members
protected:
	Entity *m_entity;
	MovementIntent m_movement_intent;

// public methods
public:
	EntityController();
	virtual ~EntityController();
	virtual void setup(Entity *controlled_entity);
	virtual void tick(flt delta_time) = 0;
	const MovementIntent &getMovementIntent() const;
};

class PlayerController: public EntityController
{
// protected members
protected:

// public methods
public:
	PlayerController();
	virtual ~PlayerController();
	virtual void tick(flt delta_time);
};

class AIController: public EntityController
{
// protected members
protected:

// public methods
public:
	AIController();
	virtual ~AIController();
	virtual void tick(flt delta_time);
};