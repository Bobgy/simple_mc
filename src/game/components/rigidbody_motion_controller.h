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
	virtual void tick(flt delta_time);
	void setup(RigidBody * parent);
	void setMovementIntent(const MovementIntent &movement_intent);
};

class HumanMotionController: public RigidBodyMotionController
{
protected:
	Radian m_turn_speed = 3.f * PI; //1000.f;
	Radian m_allowed_advance_angle = PI / 2.0f;
public:    // public members
	virtual ~HumanMotionController();
	// common interfaces
	virtual void tick(flt delta_time);
	void setTurnSpeed(Radian turn_speed) { m_turn_speed = turn_speed; }
	//inherits
	//void setup(RigidBody * parent);
	//void setMovementIntent(const MovementIntent &movement_intent);
};