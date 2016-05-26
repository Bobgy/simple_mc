#include "stdafx.h"

#include <cassert>

#include "game/rigid_body.h"
#include "game/entity.h"

bool RigidBodyController::isValid() const
{
	bool is_valid = m_entity != nullptr && m_entity->getController() != nullptr;
	return is_valid;
}

void RigidBodyController::setup(Entity *parent)
{
	m_entity = parent;
}

void RigidBodyController::tick_movement_intent(flt delta_time)
{
	RETURN_AND_WARN_IF(isValid() != true);

	EntityController *controller = m_entity->getController();
	const EntityController::MovementIntent &movement_intent =
		controller->getMovementIntent();

	Vec3f face_xz = m_entity->getRotation()->getHorizontalFacingVector();
	m_entity->give_velocity(face_xz, movement_intent.walk_intent[0]);
	m_entity->give_velocity(Vec3f{face_xz[2], 0, -face_xz[0]}, movement_intent.walk_intent[1]);

	if (bGravity) {
		m_entity->force(Vec3f{0.f, movement_intent.jump_intent, 0.f});
	} else {
		m_entity->give_velocity(Vec3f::Y_AXIS(), movement_intent.jump_intent);
	}

	if (m_entity->m_rigid_body.m_enabled_movement) {
		m_entity->m_rigid_body.m_position += m_entity->m_rigid_body.m_velocity;
		m_entity->m_rigid_body.m_velocity *= RESISTANCE;
	}
}

void RigidBodyController::tick_dynamic_collision(flt delta_time)
{
	RETURN_AND_WARN_IF(!isValid());
}

void RigidBodyController::tick_static_collision(flt delta_time)
{
	RETURN_AND_WARN_IF(!isValid());
}
