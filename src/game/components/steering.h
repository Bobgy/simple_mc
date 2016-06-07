#pragma once

#include "stdafx.h"

#include <vector>

#include "game/entity_controller.h"

#include "utility/vec.h"

class PriorityBasedAvoider : public AIController
{
// public typedef
public:
	uint32_t m_last_tick = 0;

protected:
	void calcNavForce();

public:
	/* constructor and destructor */
	PriorityBasedAvoider();
	virtual ~PriorityBasedAvoider();

	/* common interface */
	void setup(Vec3f destination);
	virtual void tick(flt delta_time);
	virtual shared_ptr<vector<Entity*>> tick_bfs(flt delta_time);
	// inherited from AIController
	// virtual bool isAI() const;


	/* getter and setter methods */

	// inherited from AIController
	// Vec3f getDestination() const;
	// void setDestination(Vec3f destination);
};