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
	template<typename T>
	void registerEventTrigger(EventBoard<T> &event_board, T key, StringID event);

public:
	// common methods

	// setup should be done after keyboard
	void setup();

	void registerEventCallback(
		StringID event_name,
		weak_ptr<CallBackFunction> call_back,
		EnumEventType event_type = EnumEventType::ON_UP);
	bool isEventActive(StringID event_name);
};