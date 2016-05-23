#pragma once

#include <memory>

#include "utility/logger.h"
#include "game/entity.h"

class Player
{
protected:
	int m_possessed_entity_index = -1;

protected:
	void possessEntity(int entity_index) {
		if (entity_index < 0) {
			LOG_WARNING(__FUNCTION__, "invalid entity_index %d\n", entity_index);
			return;
		}
		m_possessed_entity_index = entity_index;
	}

public:
	Player();
	~Player();

	// get controlled entity
	shared_ptr<Entity> getEntity();

	void setup();
};

