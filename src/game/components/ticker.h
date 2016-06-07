#pragma once

#include <vector>
#include <queue>
#include <unordered_set>

#include "utility/vec.h"

class Ticker
{
public: // public methods
	// constructor and destructor
	virtual ~Ticker() = 0;
	// common interface
	virtual void tick(flt delta_time) = 0;
};

template <typename T>
class TickerBFS : public Ticker
{
protected: // protected members
	queue<T *> m_queue;
	unordered_set<T *> m_opened;
	const vector<T *> *m_list = nullptr;
public: // public methods
	// constructor and destructor
	virtual ~TickerBFS();
	void setup(const vector<T *> *object_list) {
		m_list = object_list;
	}
	// common methods
	virtual void tick(flt delta_time);
};