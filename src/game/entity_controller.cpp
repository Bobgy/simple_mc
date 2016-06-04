#include "stdafx.h"

#include "game/entity_controller.h"

#include "game/game.h"
#include "render/render.h"
#include "utility/view.h"
#include "utility/keyboard.h"

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
	ViewController *view_controller = CurrentGame()->getViewController();
	if (view_controller->need_update()) {
		const Vec2f &rotation_speeds = view_controller->getViewRotationSpeed();
		m_movement_intent.yaw_intent = normalize_angle(m_entity->m_rigid_body.m_yaw + rotation_speeds[0]);
		m_movement_intent.pitch_intent = normalize_angle(m_entity->m_rigid_body.m_pitch + rotation_speeds[1]);
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
