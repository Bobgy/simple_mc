#include "stdafx.h"

#include "event_manager.h"
#include "game/game.h"
#include "utility/keyboard.h"

template <typename T>
void EventManager::registerEventTrigger(EventBoard<T> &event_board, T key, StringID event)
{
	m_holded_registrations.push_back(make_shared<CallBackFunction>([event, this]() {
		m_event_board.onEvent(event, EnumEventType::ON_DOWN);
	}));
	event_board.registerEventCallback(key, m_holded_registrations.back(), EnumEventType::ON_DOWN);
	m_holded_registrations.push_back(make_shared<CallBackFunction>([event, this]() {
		m_event_board.onEvent(event, EnumEventType::ON_UP);
	}));
	event_board.registerEventCallback(key, m_holded_registrations.back(), EnumEventType::ON_UP);
	LOG_INFO(
		__FUNCTION__,
		"registered a function on event %s.\n",
		GET_STRING(event).c_str());
}

void EventManager::setup()
{
	Keyboard *keyboard = CurrentGame()->getKeyboard();
	registerEventTrigger(keyboard->m_key_event_board, (uint8_t)'w', STRING_ID("forward"));
	registerEventTrigger(keyboard->m_key_event_board, (uint8_t)'s', STRING_ID("backward"));
}

void EventManager::registerEventCallback(StringID event_name, weak_ptr<CallBackFunction> call_back, EnumEventType event_type)
{
	m_event_board.registerEventCallback(event_name, call_back, event_type);
}

bool EventManager::isEventActive(StringID event_name)
{
	return m_event_board.isEventActive(event_name);
}
