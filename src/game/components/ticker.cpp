#include "stdafx.h"

#include "ticker.h"

#include "game/entity.h"

Ticker::~Ticker()
{
	// do nothing
}

template class TickerBFS<Entity>;

template <typename T>
TickerBFS<T>::~TickerBFS()
{
	// do nothing
}

template <typename T>
void TickerBFS<T>::tick(flt delta_time)
{
	if (!m_list) return;
	m_opened.clear();
	while (!m_queue.empty()) m_queue.pop();
	for (auto u : *m_list) {
		if (!u) continue;
		m_queue.push(u);
		while (!m_queue.empty()) {
			T *a = m_queue.front();
			m_queue.pop();
			shared_ptr<vector<T*>> exp = a->getController()->tick_bfs(delta_time);
			if (!exp) continue;
			for (auto b : *exp) {
				if (m_opened.find(b) == m_opened.end()) {
					m_opened.insert(b);
					m_queue.push(b);
				}
			}
		}
	}
}