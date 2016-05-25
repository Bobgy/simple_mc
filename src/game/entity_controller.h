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
		Vec2f turn_intent = { 0.0f, 0.0f };
		flt jump_intent = 0.0f;
		bool isWalking() const {
			return walk_intent || turn_intent[0];
		}
	};
protected:
	Entity *m_entity;
public:
	EntityController();
	virtual ~EntityController();
	virtual void setup(Entity *controlled_entity);
	virtual void tick(flt delta_time) = 0;
	virtual const MovementIntent &getMovementIntent() const = 0;
};

class PlayerController: public EntityController
{
protected:
	MovementIntent m_movement_intent;
public:
	PlayerController();
	virtual ~PlayerController();
	virtual void tick(flt delta_time);
	virtual const EntityController::MovementIntent &getMovementIntent() const;
};

class AIController: public EntityController
{
protected:
public:
	AIController();
	virtual ~AIController();
	virtual void tick(flt delta_time);
};