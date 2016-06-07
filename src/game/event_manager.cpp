#include "stdafx.h"

#include "event_manager.h"
#include "game/game.h"
#include "utility/keyboard.h"

void EventManager::setup()
{
	// do nothing
}

void EventManager::registerEventCallback(
	StringID event_name,
	weak_ptr<CallBackFunction> call_back,
	EnumEventType event_type,
	bool should_hold)
{
	if (should_hold) {
		auto func = call_back.lock();
		if (func) m_holded_registrations.push_back(func);
	}
	m_event_board.registerEventCallback(event_name, call_back, event_type);
}

bool EventManager::isEventActive(StringID event_name)
{
	return m_event_board.isEventActive(event_name);
}
