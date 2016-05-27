#pragma once

#include "stdafx.h"

#include "utility/event_board.h"

class EventManager {
//protected members
protected:
	EventBoard<StringID> m_event_board;
	vector<shared_ptr<CallBackFunction>> m_holded_registrations;

// protected methods
protected:
	// nothing

public:
	// common methods

	// setup should be done after keyboard
	void setup();

	void registerEventCallback(
		StringID event_name,
		weak_ptr<CallBackFunction> call_back,
		EnumEventType event_type = EnumEventType::ON_UP);
	
	template<typename T>
	void registerEventTrigger(EventBoard<T> &event_board, T key, StringID event);
	bool isEventActive(StringID event_name);
};

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