#include "stdafx.h"

#include "event_manager.h"

#include <cassert>
#include <vector>
#include <algorithm>

template <typename T>
EventBoard<T>::EventBoard()
{
}

template <typename T>
EventBoard<T>::~EventBoard()
{
}

template <typename T>
void EventBoard<T>::onEvent(T event, EnumEventType event_type)
{
	auto it = m_event_to_call_back_map.find(event);
	if (it != m_event_to_call_back_map.end()) {
		EventInfo &event_info = it->second;
		// clear expired callback functions
		auto &callbacks = event_info.m_event_type[static_cast<size_t>(event_type)];
		callbacks.erase(
			remove_if(
				callbacks.begin(),
				callbacks.end(),
				[](const weak_ptr<CallBackFunction> &p) -> bool {
					return p.expired();
				}
			),
			callbacks.end()
		);
		// call valid functions
		for (auto &ptr : callbacks) {
			auto valid_ptr = ptr.lock();
			assert(valid_ptr);
			(*valid_ptr)();
		}
	}
}

template<typename T>
void EventBoard<T>::registerEventCallback(
	T event_name,
	weak_ptr<CallBackFunction> call_back,
	EnumEventType event_type)
{
	auto it = m_event_to_call_back_map.find(event_name);
	EventInfo *container = nullptr;
	if (it == m_event_to_call_back_map.end()) {
		container = &m_event_to_call_back_map[event_name];
	} else {
		container = &it->second;
	}
	assert(container != nullptr);

	size_t event_unsigned_type = static_cast<size_t>(event_type);
	if (static_cast<size_t>(EnumEventType::COUNT) >= event_unsigned_type) {
		LOG_WARNING(__FUNCTION__, "registering an event with invalid event_type %u.\n", event_unsigned_type);
		return;
	}

	container->m_event_type[event_unsigned_type].push_back(call_back);
	LOG_INFO(
		__FUNCTION__,
		"registered a call back function on event %s with type %u.\n",
		GET_STRING(event_name).c_str(),
		event_unsigned_type
	);

	return;
}

// force instantiation
template class EventBoard<unsigned char>;
template class EventBoard<int>;
