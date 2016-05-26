#include "stdafx.h"

#include <cassert>

#include "game/rigid_body.h"
#include "game/entity.h"

#include "game/game.h"
#include "game/world.h"

bool RigidBodyController::isValid() const
{
	return m_entity != nullptr;
}

void RigidBodyController::setup(Entity *parent)
{
	m_entity = parent;
}

void RigidBodyController::tick_movement_intent(flt delta_time)
{
	RETURN_AND_WARN_IF(isValid() != true);

	EntityController *controller = m_entity->getController();
	if (controller) {
		const EntityController::MovementIntent &movement_intent =
			controller->getMovementIntent();

		Vec3f face_xz = m_entity->getRotation()->getHorizontalFacingVector();

		if (movement_intent.walk_intent[0]) {
			m_entity->give_velocity(face_xz, movement_intent.walk_intent[0]);
		}
		if (movement_intent.walk_intent[1]) {
			m_entity->give_velocity(Vec3f{face_xz[2], 0, -face_xz[0]}, movement_intent.walk_intent[1]);
		}
		
		if (movement_intent.jump_intent) {
			m_entity->force(Vec3f{0.f, movement_intent.jump_intent, 0.f});
		}
		if (movement_intent.float_intent) {
			m_entity->give_velocity(Vec3f::Y_AXIS(), movement_intent.float_intent);
		}
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
	World *world = CurrentGame()->getWorld();
	RETURN_AND_WARN_IF(world == nullptr);

	Entity &entity = *m_entity;
	Vec3i p;
	for (int i = 0; i < 3; ++i)
		p[i] = floor(entity[i]);
	p[1] = floor(entity[1] + 0.5*entity.getHeight());
	entity.on_ground = false;
	for (int dx = -1; dx <= 1; ++dx) {
		for (int dy = -2; dy <= 2; ++dy) {
			for (int dz = -1; dz <= 1; ++dz) {
				map<Vec3i, Block*>::const_iterator it;
				if ((it = world->find(Vec3i{p[0] + dx, p[1] + dy, p[2] + dz})) != world->end()) {
					m_entity->on_ground |= m_entity->collide_cube_vertically(it->first);
					m_entity->collide_cube_horizontally(it->first);
				}
			}
		}
	}
	if (m_entity->on_ground) m_entity->be_slowed(smoothness_ground);
}
