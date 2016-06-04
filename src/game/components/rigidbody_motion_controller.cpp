#include "stdafx.h"

#include "rigidbody_motion_controller.h"

#include "game/common_structures.h"
#include "game/rigid_body.h"
#include "game/entity_controller.h"

RigidBodyMotionController::~RigidBodyMotionController()
{
	// do nothing
}

void RigidBodyMotionController::tick(flt delta_time)
{
	m_rigidbody->m_yaw = m_movement_intent.yaw_intent;

	Vec3f face_xz = horizontal_facing_vector(m_rigidbody->m_yaw);
	if (m_movement_intent.walk_intent[0]) {
		m_rigidbody->give_velocity(face_xz, m_movement_intent.walk_intent[0]);
	}
	if (m_movement_intent.walk_intent[1]) {
		m_rigidbody->give_velocity(Vec3f{face_xz[2], 0, -face_xz[0]}, m_movement_intent.walk_intent[1]);
	}

	if (m_movement_intent.jump_intent) {
		m_rigidbody->force(Vec3f{0.f, m_movement_intent.jump_intent, 0.f} * delta_time);
	}
}

void RigidBodyMotionController::setup(RigidBody * parent)
{
	m_rigidbody = parent;
}

void RigidBodyMotionController::setMovementIntent(const MovementIntent &movement_intent)
{
	m_movement_intent = movement_intent;
}
