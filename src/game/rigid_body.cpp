#include "stdafx.h"

#include <cassert>

#include "game/rigid_body.h"

bool RigidBodyController::isValid() const
{
	return m_rigid_body != nullptr && m_movement_intent != nullptr;
}

void RigidBodyController::setup(
	RigidBody *rigid_body,
	const EntityController::MovementIntent *movement_intent)
{
	m_rigid_body = rigid_body;
	m_movement_intent = movement_intent;
}

void RigidBodyController::tick_movement_intent(flt delta_time)
{
	RETURN_AND_WARN_IF(!isValid());
}

void RigidBodyController::tick_dynamic_collision(flt delta_time)
{
	RETURN_AND_WARN_IF(!isValid());
}

void RigidBodyController::tick_static_collision(flt delta_time)
{
	RETURN_AND_WARN_IF(!isValid());
}
