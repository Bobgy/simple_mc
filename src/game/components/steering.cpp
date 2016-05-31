#include "stdafx.h"

#include "steering.h"

#include "game/game.h"
#include "game/world.h"
#include "game/gridmap.h"

void PriorityBasedAvoider::calcNavForce()
{
	Vec3f facing = horizontal_facing_vector(AIController::m_movement_intent.yaw_intent);
	Vec2f f2 = horizontal_projection(facing);
	f2 = f2 * m_movement_intent.walk_intent[0] + AIController::m_entity->m_nav_force;
	m_movement_intent.yaw_intent = get_yaw_from_direction(f2);
	m_movement_intent.walk_intent[0] = min(k_ai_speed, !f2);
}

PriorityBasedAvoider::PriorityBasedAvoider()
{
}

PriorityBasedAvoider::~PriorityBasedAvoider()
{
}

void PriorityBasedAvoider::setup(Vec3f destination)
{
}

void PriorityBasedAvoider::tick(flt delta_time)
{
	uint32_t current_tick = CurrentGame()->getTickCount();
	if (m_last_tick == current_tick) return;
	m_last_tick = current_tick;
	/* start to tick */
	
	AIController::tick(delta_time);
	calcNavForce();

	World *world = CurrentGame()->getWorld();
	RETURN_IF_NULL(world);
	RigidBody &body1 = m_entity->m_rigid_body;
	GridMap *gridmap = world->getGridMap();
	RETURN_AND_WARN_IF(gridmap == nullptr);

	for (flt ahead_ratio : {1.0f, 20.0f}) {
		RigidBody body_ahead = body1;
		body_ahead.m_position += body_ahead.m_velocity * delta_time * ahead_ratio;
		//body.m_position += m_movement_intent.walk_intent[0] * delta_time;
		body_ahead.m_shape.getCylinder()->r *= k_avoidance_radius_ratio;
		Vec3i ip = round(body_ahead.m_position);
		Vec2i ip2 = horizontal_projection(ip);
		vector<Entity *> pending_entities;
		Entity::Priority priority = m_entity->getPriority();
		++priority.m_propagated_steps;
		gridmap->iterateGridsInRange(
			ip2 - 1,
			ip2 + 1,
			[&body_ahead, &pending_entities, priority, current_tick, delta_time, ahead_ratio, this](Vec2i p, Grid *grid) {
			assert(grid);
			for (auto &entity : grid->m_entities) {
				if (!entity) continue;
				if (entity == m_entity) continue;
				if (entity->getPriority() < m_entity->getPriority()) continue;
				RigidBody &body2 = entity->m_rigid_body;

				// TODO: may be zero
				flt cos_val = body2.m_velocity.normalize() * body_ahead.m_velocity.normalize();
				if ( cos_val > cos(PI / 6.0) ) continue;

				RigidBody body_ahead2 = body2;
				//body_ahead2.m_position += body_ahead2.m_velocity * delta_time * ahead_ratio;
				flt len = body_ahead.intersect(body_ahead2);
				if (sgn(len)) {
					if (find(pending_entities.begin(), pending_entities.end(), entity) == pending_entities.end()) {
						Vec2f dir_vec =
							horizontal_projection(body_ahead.m_position) -
							horizontal_projection(body_ahead2.m_position);
						if (sgn(sqr(dir_vec)) == 0) continue;

						flt sin_val = (body_ahead.m_shape.getCylinder()->r + body_ahead2.m_shape.getCylinder()->r) / !dir_vec;
						sin_val = min(sin_val, 1.0f);
						flt ang = -asin(sin_val);
						dir_vec = dir_vec.normalize();
						if (body2.m_velocity ^ dir_vec > 0) dir_vec = dir_vec.rotate(ang);
						else dir_vec = dir_vec.rotate(-ang);

						flt F = k_steering_force * min(1.0f, (len + 0.1f) * 3.f) * 20.0f / (20.f + ahead_ratio);
						entity->m_nav_force -= F * dir_vec;
						pending_entities.push_back(entity);
						entity->setTemporaryPriority({priority, current_tick + k_temporary_priority_last_ticks});
					}
				}
			}
			for (auto entity_ptr : pending_entities) {
				entity_ptr->tick(delta_time);
			}
		});
	}
}
