#pragma once

#include "utility/vec.h"

class Entity;

class EntityController
{
protected:
	Entity *m_entity;
public:
	EntityController();
	virtual ~EntityController() = 0;
	virtual void setup(Entity *controlled_entity);
	virtual void tick(flt delta_time) = 0;
};

class PlayerController: EntityController
{
protected:
public:
	PlayerController();
	virtual ~PlayerController();

	virtual void tick(flt delta_time);
};