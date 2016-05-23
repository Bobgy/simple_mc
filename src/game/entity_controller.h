#pragma once

#include "utility/vec.h"

class Entity;

class EntityController
{
protected:
	Entity *m_entity;
public:
	EntityController();
	virtual ~EntityController();
	virtual void setup(Entity *controlled_entity);
	virtual void tick(flt delta_time) = 0;
};

class PlayerController: public EntityController
{
protected:
public:
	PlayerController();
	virtual ~PlayerController();
	virtual void tick(flt delta_time);
};

class AIController: public EntityController
{
protected:
public:
	AIController();
	virtual ~AIController();
	virtual void tick(flt delta_time);
};