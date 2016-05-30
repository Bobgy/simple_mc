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

	RigidBody body = body1;
	body.m_position += m_movement_intent.walk_intent[0] * delta_time;
	body.m_shape.getCylinder()->r *= k_avoidance_radius_ratio;
	Vec3i ip = round(body.m_position);
	Vec2i ip2 = horizontal_projection(ip);
	vector<Entity *> pending_entities;
	Entity::Priority priority = m_entity->getPriority();
	++priority.m_propagated_steps;
	gridmap->iterateGridsInRange(
		ip2 - 1,
		ip2 + 1,
		[&body, &pending_entities, priority, current_tick, this](Vec2i p, Grid *grid) {
			assert(grid);
			for (auto &entity : grid->m_entities) {
				if (!entity) continue;
				if (entity == m_entity) continue;
				if (entity->getPriority() < m_entity->getPriority()) continue;
				RigidBody &body2 = entity->m_rigid_body;
				if (body2.m_velocity.normalize() * body.m_velocity.normalize() < cos(30.f)) continue;
				flt len = body.intersect(body2);
				if (sgn(len)) {
					Vec2f dir_vec =
						horizontal_projection(body.m_position) -
						horizontal_projection(body2.m_position);
					if (sgn(sqr(dir_vec)) == 0) {
						uniform_real_distribution<> dis(-PI, PI);
						flt ang = dis(k_pseudo_gen);
						dir_vec = {cos(ang), sin(ang)};
					}
					dir_vec = dir_vec.normalize();
					flt F = min(k_steering_force, (len + 0.5f) * k_steering_force);
					entity->m_nav_force -= F * dir_vec;
					pending_entities.push_back(entity);
					entity->setTemporaryPriority({priority, current_tick});
				}
			}
		}
	);
	for (auto entity_ptr : pending_entities) {
		entity_ptr->tick(delta_time);
	}
}
