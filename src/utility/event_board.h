#pragma once

#include "stdafx.h"

#include <unordered_map>

typedef function<void(void)> CallBackFunction;

enum class EnumEventType : size_t
{
	ON_UP = 0,
	ON_DOWN = 1,
	COUNT = 2
};

template <typename T>
class EventBoard
{
// protected members
protected:
	struct EventInfo {
		bool is_active = false;
		vector<weak_ptr<CallBackFunction>> m_event_type[static_cast<size_t>(EnumEventType::COUNT)];
	};
	typename unordered_map<T, EventInfo> m_event_to_call_back_map;

// public methods
public:
	EventBoard();
	~EventBoard();

	void clear() { m_event_to_call_back_map.clear(); }
	void EventBoard::onEvent(T event, EnumEventType event_type);

	// register a call back function to an event
	// the call back should be passed as a weak_ptr
	// invalid weak_ptr will be automatically unregistered
	void registerEventCallback(
		T event_name,
		weak_ptr<CallBackFunction> call_back,
		EnumEventType event_type = EnumEventType::ON_UP);
	bool isEventActive(T event_name);
};