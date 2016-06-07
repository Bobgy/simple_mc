#include "stdafx.h"

#include <cassert>
#include <random>

#include "game/rigid_body.h"
#include "game/entity.h"

#include "game/game.h"
#include "game/world.h"
#include "game/gridmap.h"
#include "game/entity_controller.h"
#include "game/components/rigidbody_motion_controller.h"

bool RigidBodyController::isValid() const
{
	return m_entity != nullptr && m_entity->isTicking();
}

void RigidBodyController::setup(Entity *parent, RigidBodyMotionController *motion_controller)
{
	m_entity = parent;
	m_motion_controller = motion_controller;
}

void RigidBodyController::tick_movement_intent(flt delta_time)
{
	RETURN_IF(isValid() != true);

	EntityController *controller = m_entity->getController();
	if (controller) {
		const MovementIntent &movement_intent =
			controller->getMovementIntent();
		if (m_motion_controller) {
			m_motion_controller->setMovementIntent(movement_intent);
			m_motion_controller->tick(delta_time);
		}
		m_entity->m_rigid_body.m_pitch = movement_intent.pitch_intent;
		if (movement_intent.float_intent) {
			m_entity->m_rigid_body.giveVelocity(Vec3f::Y_AXIS(), movement_intent.float_intent);
		}
	}
}

void RigidBodyController::tick_physical_simulation(flt delta_time)
{
	RETURN_IF(isValid() != true);

	RigidBody &rigidbody = m_entity->m_rigid_body;
	if (rigidbody.m_enabled_movement) {
		rigidbody.m_position += m_entity->m_rigid_body.m_velocity * delta_time;
		rigidbody.m_velocity *= RESISTANCE;
		if (bGravity && rigidbody.m_affected_by_gravity) {
			rigidbody.m_velocity[1] -= GRAVITY * delta_time;
		}
	}
}

void RigidBodyController::tick_dynamic_collision(flt delta_time)
{
	RETURN_IF(!isValid());
	RigidBody &body1 = m_entity->m_rigid_body;
#ifdef _USE_ENTITY_MAP
	auto &entity_map = CurrentGame()->getWorld()->getEntityMap();
	for (auto &ptr: entity_map) {
		auto entity = ptr.second.lock();
		if (!entity) continue;
		RigidBody &body2 = entity->m_rigid_body;
		flt len = body1.intersect(body2);
		if (sgn(len)) {
			Vec2f dir_vec =
				horizontal_projection(body1.m_position) -
				horizontal_projection(body2.m_position);
			if (sgn(sqr(dir_vec)) == 0) {
				uniform_real_distribution<> dis(-PI, PI);
				flt ang = dis(k_pseudo_gen);
				dir_vec = {cos(ang), sin(ang)};
			}
			Vec3f d = as_horizontal_projection(dir_vec.normalize());
			flt F = min(3.0f, (len + 0.2f) * 3.0f) * delta_time;
			body1.collision_force(d, F);
			body2.collision_force(-1.0f * d, F);
		}
	}
#else
	World *world = CurrentGame()->getWorld();
	RETURN_IF_NULL(world);
	GridMap *gridmap = world->getGridMap();
	RETURN_AND_WARN_IF(gridmap == nullptr);

	Vec3i ip = body1.getCenterCoord();
	Vec2i ip2 = horizontal_projection(ip);
	gridmap->iterateGridsInRange(ip2 - 1, ip2 + 1, [&body1, delta_time, this](Vec2i p, Grid *grid) {
		assert(grid);
		for (auto &entity : grid->m_entities) {
			if (!entity) continue;
			if (entity == m_entity) continue;
			RigidBody &body2 = entity->m_rigid_body;
			flt len = body1.intersect(body2);
			if (sgn(len)) {
				Vec2f dir_vec =
					horizontal_projection(body1.m_position) -
					horizontal_projection(body2.m_position);
				if (sgn(sqr(dir_vec)) == 0) {
					uniform_real_distribution<> dis(-PI, PI);
					flt ang = dis(k_pseudo_gen);
					dir_vec = {cos(ang), sin(ang)};
				}
				Vec3f d = as_horizontal_projection(dir_vec.normalize());
				flt F = min(k_collision_force, (len + 0.3f) * k_collision_force) * delta_time;
				body1.collision_force(d, F);
			}
		}
	});
#endif
}

void RigidBodyController::tick_static_collision(flt delta_time)
{
	RETURN_IF(!isValid());
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
				if ((it = world->find(Vec3i{p[0] + dx, p[1] + dy, p[2] + dz})) != world->blocks_end()) {
					m_entity->on_ground |= m_entity->collide_cube_vertically(it->first);
					m_entity->collide_cube_horizontally(it->first);
				}
			}
		}
	}
	if (m_entity->on_ground) m_entity->m_rigid_body.beSlowed(smoothness_ground);
}

flt RigidBody::intersect(const RigidBody & r) const
{
	const RigidBody *a = this, *b = &r;
	if ((uint8_t)a->m_shape.m_type > (uint8_t)b->m_shape.m_type) swap(a, b);
	typedef Shape::Type Enum;
	switch (a->m_shape.m_type)
	{
		case Enum::CYLINDER:
		{
			const Shape::Cylinder *a_cyl = a->m_shape.getCylinder();
			assert(a_cyl);
			switch (b->m_shape.m_type)
			{
				case Enum::CYLINDER:
				{
					const Shape::Cylinder *b_cyl = b->m_shape.getCylinder();
					assert(b_cyl);
					const Vec3f &ap = a->m_position, &bp = b->m_position;
					if (sgn(seg_intersect(ap[1], ap[1] + a_cyl->h, bp[1], bp[1] + b_cyl->h))) {
						Vec2f au = horizontal_projection(ap), bu = horizontal_projection(bp);
						flt len = !(au - bu);
						return max(a_cyl->r + b_cyl->r - len, 0.0f);
					}
					return 0.0f;
				}
				case Enum::CUBE:
				{
					LOG_WARNING(__FUNCTION__, "intersection between CYLINDER and CUBE is not implemented!\n");
					return 0.0f; // TODO
				}
				default:
				{
					LOG_WARNING(__FUNCTION__, "r has unknown shape type.\n");
				}
			}
		}

		case Enum::CUBE:
		{
			switch (b->m_shape.m_type) {
			case Enum::CUBE:
			{
				LOG_WARNING(__FUNCTION__, "intersection between CUBEs is not implemented!\n");
				return 0.0f; // TODO
			}
			default:
			{
				LOG_WARNING(__FUNCTION__, "r has unknown shape type.\n");
			}
			}
		}
		default:
		{
			LOG_WARNING(__FUNCTION__, "this has unknown shape type.\n");
		}
	}
	return 0.0f;
}
