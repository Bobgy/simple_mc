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

const EntityController::MovementIntent &EntityController::getMovementIntent() const
{
	return m_movement_intent;
}

PlayerController::PlayerController()
{
}

PlayerController::~PlayerController()
{
}

flt step = 0.3f, eps = 1e-8f;
void PlayerController::tick(flt delta_time)
{
	ViewController *view_controller = CurrentGame()->getViewController();
	if (view_controller->need_update()) {
		const Vec2f &rotation_speeds = view_controller->getViewRotationSpeed();
		const Rotation *rotations = m_entity->getRotation();
		Vec2f rot = rotation_speeds + rotations->getVec2();
		rot[1] = clamp(rot[1], -PI*0.49f, PI*0.3f);
		m_entity->setRotation(rot);
	}

	// tick keyboard
	const Keyboard &keyboard = *CurrentGame()->getKeyboard();
	int32_t df = 0;
	EventManager *event_manager = CurrentGame()->getEventManager();
	
	// forward movement intent
	df = (int32_t)event_manager->isEventActive(STRING_ID("forward")) - (int32_t)event_manager->isEventActive(STRING_ID("backward"));
	m_movement_intent.walk_intent[0] = step * (flt)df;
	
	// left movement intent;
	df = (int32_t)keyboard.get_state('a') - (int32_t)keyboard.get_state('d');
	m_movement_intent.walk_intent[1] = step * 0.5f * (flt)df;
	
	if (bGravity) {
		if (keyboard.get_state(' ') && m_entity->on_ground) {
			m_movement_intent.jump_intent = 13.f;
		} else {
			m_movement_intent.jump_intent = 0.0f;
		}
		m_entity->force(Vec3f{0.f, m_movement_intent.jump_intent, 0.f});
	} else {
		df = (int32_t)keyboard.get_state(' ') - (int32_t)keyboard.get_special_state(GLUT_KEY_SHIFT_L);
		m_movement_intent.jump_intent = 0.5f * (flt)df;
	}
	/*
	Vec3f face_xz = m_entity->getRotation()->getHorizontalFacingVector();
	m_entity->give_velocity(face_xz, m_movement_intent.walk_intent[0]);
	m_entity->give_velocity(Vec3f{face_xz[2], 0, -face_xz[0]}, m_movement_intent.walk_intent[1]);
	m_entity->give_velocity(Vec3f::Y_AXIS(), m_movement_intent.jump_intent);
	*/
}

AIController::AIController()
{
	// do nothing
}

AIController::~AIController()
{
	// do nothing
}

void AIController::tick(flt delta_time)
{
	// LOG_INFO(__FUNCTION__, "AI is ticking.\n");
}
