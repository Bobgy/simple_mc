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
		m_rigidbody->giveVelocity(face_xz, m_movement_intent.walk_intent[0]);
	}
	if (m_movement_intent.walk_intent[1]) {
		m_rigidbody->giveVelocity(Vec3f{face_xz[2], 0, -face_xz[0]}, m_movement_intent.walk_intent[1]);
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

HumanMotionController::~HumanMotionController()
{
	// do nothing
}

void HumanMotionController::tick(flt delta_time)
{
	Radian delta = (m_movement_intent.yaw_intent - m_rigidbody->m_yaw).normalize();
	if (fabs(delta) <= m_turn_speed * delta_time) {
		m_rigidbody->m_yaw = m_movement_intent.yaw_intent;
	} else {
		m_rigidbody->m_yaw += (delta > 0.f ? m_turn_speed : -m_turn_speed) * delta_time;
		m_rigidbody->m_yaw.normalize();
	}

	Vec2f desired_velocity = m_movement_intent.getDesiredVelocity();
	if (m_rigidbody->m_yaw.absDelta(m_movement_intent.yaw_intent) < m_allowed_advance_angle) {
		m_rigidbody->giveVelocity(as_horizontal_projection(desired_velocity), 1.0f);
	}

	if (m_movement_intent.jump_intent) {
		m_rigidbody->force(Vec3f{0.f, m_movement_intent.jump_intent, 0.f} *delta_time);
	}
}
