#include "stdafx.h"

#include "event_board.h"

#include <cassert>
#include <vector>
#include <algorithm>

#include "utility/string_id.h"

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
	EventInfo *event_info = nullptr;
	if (it == m_event_to_call_back_map.end()) {
		event_info = &m_event_to_call_back_map[event];
	} else {
		event_info = &it->second;
	}
	if (event_info != nullptr) {
		bool changed = false;
		if (event_type == EnumEventType::ON_UP) {
			changed = event_info->is_active != false;
			event_info->is_active = false;
		} else {
			changed = event_info->is_active != true;
			event_info->is_active = true;
		}

		if (changed) {
			// clear expired callback functions
			auto &callbacks = event_info->m_event_type[static_cast<size_t>(event_type)];
			callbacks.erase(remove_if(
				callbacks.begin(), callbacks.end(),
				[](const weak_ptr<CallBackFunction> &p) -> bool {
					return p.expired();
				}),
				callbacks.end());
			// call valid functions
			for (auto &ptr : callbacks) {
				auto valid_ptr = ptr.lock();
				assert(valid_ptr);
				(*valid_ptr)();
			}
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
	if (event_unsigned_type >= static_cast<size_t>(EnumEventType::COUNT)) {
		LOG_WARNING(__FUNCTION__, "registering an event with invalid event_type %u.\n", event_unsigned_type);
		return;
	}

	container->m_event_type[event_unsigned_type].push_back(call_back);
	return;
}

template<typename T>
bool EventBoard<T>::isEventActive(T event_name)
{
	auto it = m_event_to_call_back_map.find(event_name);
	if (it == m_event_to_call_back_map.end()) {
		return false;
	}
	return it->second.is_active;
}

// force instantiation
template class EventBoard<unsigned char>;
template class EventBoard<int>;
template class EventBoard<StringID>;