#include "stdafx.h"

#include "game/entity_controller.h"

#include "game/game.h"
#include "render/render.h"
#include "utility/view.h"
#include "utility/keyboard.h"
#include "game/gridmap.h"

EntityController::EntityController(): m_entity(nullptr)
{
}

EntityController::~EntityController()
{
}

void EntityController::setup(Entity *controlled_entity)
{
	m_entity = controlled_entity;
}

std::shared_ptr<std::vector<Entity*>> EntityController::tick_bfs(flt delta_time)
{
	if (!m_entity->isTicking()) return nullptr;
	tick(delta_time);
	return nullptr;
}

bool EntityController::isAI() const
{
	return false;
}

const MovementIntent &EntityController::getMovementIntent() const
{
	return m_movement_intent;
}

PlayerController::PlayerController()
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::tick(flt delta_time)
{
	if (!m_entity->isTicking()) return;

	ViewController *view_controller = CurrentGame()->getViewController();
	if (view_controller->need_update()) {
		const Vec2f &rotation_speeds = view_controller->getViewRotationSpeed();
		Radian &yaw_intent = m_movement_intent.yaw_intent;
		yaw_intent += rotation_speeds[0];
		yaw_intent = yaw_intent.normalize();
		Radian &pitch_intent = m_movement_intent.pitch_intent;
		pitch_intent += rotation_speeds[1];
		pitch_intent = pitch_intent.normalize();
	}

	// tick keyboard
	const Keyboard &keyboard = *CurrentGame()->getKeyboard();
	int32_t df = 0;
	EventManager *event_manager = CurrentGame()->getEventManager();
	
	// forward movement intent
	df = (int32_t)event_manager->isEventActive(STRING_ID("forward")) - (int32_t)event_manager->isEventActive(STRING_ID("backward"));
	m_movement_intent.walk_intent[0] = k_ai_speed * (flt)df;
	
	// left movement intent;
	df = (int32_t)keyboard.get_state('a') - (int32_t)keyboard.get_state('d');
	m_movement_intent.walk_intent[1] = k_ai_speed * 0.5f * (flt)df;
	
	if (bGravity) {
		if (keyboard.get_state(' ') && m_entity->on_ground) {
			m_movement_intent.jump_intent = 25.f;
		} else {
			m_movement_intent.jump_intent = 0.0f;
		}
		m_movement_intent.float_intent = 0.0f;
	} else {
		m_movement_intent.jump_intent = 0.0f;
		df = (int32_t)keyboard.get_state(' ') - (int32_t)keyboard.get_special_state(GLUT_KEY_SHIFT_L);
		m_movement_intent.float_intent = k_float_speed * (flt)df;
	}
}

std::shared_ptr<std::vector<Entity*>> PlayerController::tick_bfs(flt delta_time)
{
	if (!m_entity->isTicking()) return nullptr;

	uint32_t current_tick = CurrentGame()->getTickCount();
	if (m_last_tick == current_tick) return nullptr;
	m_last_tick = current_tick;
	/* start to tick */

	tick(delta_time);

	World *world = CurrentGame()->getWorld();
	RETURN_STH_IF_NULL(world, nullptr);

	RigidBody &body1 = m_entity->m_rigid_body;
	GridMap *gridmap = world->getGridMap();
	RETURN_STH_AND_WARN_IF(gridmap == nullptr, nullptr);

	shared_ptr<vector<Entity *>> pending_entities;
	pending_entities = make_shared<vector<Entity *>>();
	assert(pending_entities);

	for (flt ahead_ratio : {15.0f, 1.0f}) {
		RigidBody body_ahead = body1;
		body_ahead.m_position += body_ahead.m_velocity * delta_time * ahead_ratio;
		//body.m_position += m_movement_intent.walk_intent[0] * delta_time;
		if (ahead_ratio != 1.0f) {
			body_ahead.m_shape.getCylinder()->r *= k_avoidance_radius_ratio;
		}
		Vec3i ip = round(body_ahead.m_position);
		Vec2i ip2 = horizontal_projection(ip);

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
				if (bPriorityEnabled) {
					if (entity->getPriority() < m_entity->getPriority()) continue;
				}
				RigidBody &body2 = entity->m_rigid_body;

				// do not collide with entities moving towards the same direction
				/*
				if (sgn(sqr(body2.m_velocity)) > 0 && sgn(sqr(body_ahead.m_velocity)) > 0) {
				flt cos_val = body2.m_velocity.normalize() * body_ahead.m_velocity.normalize();
				if (cos_val > cos(PI / 6.0)) continue;
				}
				*/

				{
					Vec3f v_relative = body_ahead.m_velocity - body2.m_velocity;
					Vec3f p_relative = body2.m_position - body_ahead.m_position;
					if (sgn(sqr(v_relative)) && sgn(sqr(p_relative))) {
						if (v_relative.normalize() * p_relative.normalize() < cos(PI / 3.0f)) continue;
					}
				}

				RigidBody &body_ahead2 = body2;
				//body_ahead2.m_position += body_ahead2.m_velocity * delta_time * ahead_ratio;
				flt len = body_ahead.intersect(body_ahead2);
				if (sgn(len)) {
					assert(sgn(len) > 0);
					if (find(pending_entities->begin(), pending_entities->end(), entity) == pending_entities->end()) {
						Vec2f dir_vec =
							horizontal_projection(body_ahead.m_position) -
							horizontal_projection(body_ahead2.m_position);
						if (sgn(sqr(dir_vec)) == 0) continue;

						flt sin_val = (body_ahead.m_shape.getCylinder()->r + body_ahead2.m_shape.getCylinder()->r) / !dir_vec;
						sin_val = min(sin_val * 1.2f, 1.0f);
						flt ang = asin(sin_val);
						dir_vec = dir_vec.normalize();

						/*
						if (body2.m_velocity ^ dir_vec > 0) dir_vec = dir_vec.rotate(ang);
						else dir_vec = dir_vec.rotate(-ang);
						//*/

						flt F = k_steering_force * min(1.0f, (len + 0.1f) * 3.f) * 20.0f / (20.f + ahead_ratio);
						entity->m_nav_force -= F * dir_vec;

						if (bPriorityEnabled) {
							pending_entities->push_back(entity);
							uint32_t expire_tick = m_entity->getTemporaryExpireTick();
							if (current_tick >= expire_tick) {
								entity->setTemporaryPriority({priority, current_tick + k_temporary_priority_last_ticks});
							} else {
								entity->setTemporaryPriority({priority, expire_tick});
							}
						}
					}
				}
			}
		});

	}

	return pending_entities;
}

AIController::AIController()
{
	// do nothing
}

AIController::~AIController()
{
	// do nothing
}

void AIController::setup(Vec3f destination)
{
	m_destination = destination;
}

const flt destination_torlerance = 2.0f;

void AIController::tick(flt delta_time)
{
	/*
	Entity *player = CurrentGame()->getPlayerEntity();
	if (player) {
		m_destination = player->m_rigid_body.m_position;
	}
	*/

	RETURN_IF_NULL(m_entity);
	if (!m_entity->isTicking()) return;

	RigidBody &rigid_body = m_entity->m_rigid_body;
	Vec2f goal_vector = horizontal_projection(m_destination - rigid_body.m_position);
	flt yaw = get_yaw_from_direction(goal_vector);
	if (sqr(goal_vector) < sqr(destination_torlerance)) {
		m_movement_intent.walk_intent = Vec2f::ZERO();
		m_movement_intent.yaw_intent = yaw;
	} else {
		if (abs_delta_angle(yaw, rigid_body.m_yaw) < 1e-2f) {
			m_movement_intent.walk_intent[0] = k_ai_speed;
		} else {
			m_movement_intent.yaw_intent = yaw;
			m_movement_intent.walk_intent[0] = k_ai_speed;
		}
	}
}

bool AIController::isAI() const
{
	return true;
}

bool AIController::hasArrivedDestination(const RigidBody & rigid_body, Vec3f destination)
{
	Vec3f dis = rigid_body.m_position - destination;
	dis[1] = 0;
	return (sqr(dis) < sqr(destination_torlerance));
}

Vec3f AIController::getDestination() const
{
	return m_destination;
}

void AIController::setDestination(Vec3f destination)
{
	m_destination = destination;
}
