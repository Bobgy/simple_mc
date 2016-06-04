#pragma once

#include "utility/vec.h"
#include "game/entity_controller.h"

class RigidBody;

class RigidBodyMotionController
{
protected: // protected members
	RigidBody *m_rigidbody;
	MovementIntent m_movement_intent;
public:    // public members
	virtual ~RigidBodyMotionController();
	// common interfaces
	void tick(flt delta_time);
	void setup(RigidBody * parent);
	void setMovementIntent(const MovementIntent &movement_intent);
};