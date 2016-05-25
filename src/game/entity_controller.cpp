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

PlayerController::PlayerController()
{
}

PlayerController::~PlayerController()
{
}

flt step = 0.3, eps = 1e-8;
void PlayerController::tick(flt delta_time)
{
	ViewController *view_controller = CurrentGame()->getViewController();
	if (view_controller->need_update()) {
		const pair<flt, flt> &rotation_speeds = view_controller->getViewRotationSpeed();
		const Rotation *rotations = m_entity->getRotation();
		m_entity->setRotation(rotation_speeds.first + rotations->getH(), rotation_speeds.second + rotations->getV());
	}

	// tick keyboard
	const Keyboard &keyboard = *CurrentGame()->getKeyboard();
	int32_t df = 0;
	EventManager *event_manager = CurrentGame()->getEventManager();
	{
		df = (int32_t)event_manager->isEventActive(STRING_ID("forward")) - (int32_t)event_manager->isEventActive(STRING_ID("backward"));
		m_entity->give_velocity(m_entity->getRotation()->getHorizontalFacingVector(), step*df);
	}
	/*
	if (keyboard.get_state('w') ^ keyboard.get_state('s')) {
		df = keyboard.get_state('w') ? 1 : -1;
		m_entity->give_velocity(m_entity->getRotation()->getHorizontalFacingVector(), step*df);
	}
	*/
	if (keyboard.get_state('a') ^ keyboard.get_state('d')) {
		df = keyboard.get_state('a') ? 1 : -1;
		Vec3f face_xz = m_entity->getRotation()->getHorizontalFacingVector();
		m_entity->give_velocity(Vec3f(face_xz[2], 0, -face_xz[0]), step*0.5*df);
	}
	if (bGravity) {
		if (keyboard.get_state(' ') && m_entity->on_ground) {
			m_entity->force(Vec3f(0, 13, 0));
		}
	} else {
		if (keyboard.get_state(' '))
			m_entity->give_velocity(Vec3f(0, 0.5, 0), 1);
		if (keyboard.get_special_state(GLUT_KEY_SHIFT_L))
			m_entity->give_velocity(Vec3f(0, -0.5, 0), 1);
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

void AIController::tick(flt delta_time)
{
	LOG_INFO(__FUNCTION__, "AI is ticking.\n");
}
