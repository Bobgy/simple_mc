#include "stdafx.h"

#include <cassert>
#include <random>

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

		m_entity->m_rigid_body.m_yaw = movement_intent.yaw_intent;
		m_entity->m_rigid_body.m_pitch = movement_intent.pitch_intent;
		
		Vec3f face_xz = m_entity->m_rigid_body.getHorizontalFacingVector();

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
	auto &entity_map = CurrentGame()->getWorld()->getEntityMap();
	RigidBody &body1 = m_entity->m_rigid_body;
	//Vec3i ip = body1.getCenterCoord();
	//auto it = entity_map.lower_bound(Vec3i{INT32_MIN, ip[1], INT32_MIN});
	//Vec3i ub = Vec3i{INT32_MIN, ip[1] + 1, INT32_MIN};
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
