#pragma once

#include "utility/vec.h"
#include "game/common_structures.h"

class Entity;
class RigidBody;

class EntityController
{
// protected members
protected:
	Entity *m_entity;
	MovementIntent m_movement_intent;

// public methods
public:
	EntityController();
	virtual ~EntityController();
	virtual void setup(Entity *controlled_entity);
	virtual void tick(flt delta_time) = 0;
	virtual shared_ptr<vector<Entity*>> tick_bfs(flt delta_time);
	virtual bool isAI() const;
	const MovementIntent &getMovementIntent() const;
};

class PlayerController: public EntityController
{
// protected members
protected:
	uint32_t m_last_tick;
// public methods
public:
	PlayerController();
	virtual ~PlayerController();
	virtual void tick(flt delta_time);
	virtual shared_ptr<vector<Entity*>> tick_bfs(flt delta_time);
};

class AIController: public EntityController
{
// protected members
protected:
	Vec3f m_destination, m_stay_position;

// public methods
public:
	/* constructor and destructor */
	AIController();
	virtual ~AIController();

	/* common interface */
	void setup(Vec3f destination);
	virtual void tick(flt delta_time);
	//inherits
	//virtual shared_ptr<vector<Entity*>> tick_bfs(flt delta_time);
	virtual bool isAI() const;

	/* static helper */
	static bool hasArrivedDestination(const RigidBody &rigid_body, Vec3f destination);

	/* getter and setter methods */
	Vec3f getDestination() const;
	void setDestination(Vec3f destination);
};