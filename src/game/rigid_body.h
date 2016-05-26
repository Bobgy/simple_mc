#pragma once

#include "stdafx.h"

#include <cassert>

struct Shape
{
	enum class Type: uint8_t {
		UNKNOWN = 0,
		CYLINDER = 1,
		CUBE = 2
	} m_type = Type::UNKNOWN;

	struct Cylinder {
		flt r, h; // a cylinder with radius r and height h.
	};
	struct Cube {
		flt x, y, z; // width on x, y, z axes
	};
	union {
		Cylinder m_cylinder;
		Cube m_cube;
	};

	/*==========  getter and setter ===========*/
	void setCylinder(flt r, flt h) {
		m_type = Type::CYLINDER;
		m_cylinder.r = r;
		m_cylinder.h = h;
	}
	Cylinder *getCylinder() {
		if (m_type != Type::CYLINDER) return nullptr;
		return &m_cylinder;
	}
	const Cylinder *getCylinder() const {
		if (m_type != Type::CYLINDER) return nullptr;
		return &m_cylinder;
	}

	void setCube(flt x, flt y, flt z) {
		m_type = Type::CUBE;
		m_cube.x = x;
		m_cube.y = y;
		m_cube.z = z;
	}
	Cube *getCube() {
		if (m_type != Type::CUBE) return nullptr;
		return &m_cube;
	}
	const Cube *getCube() const {
		if (m_type != Type::CUBE) return nullptr;
		return &m_cube;
	}
};

class RigidBody
{
// public members
public:
	Vec3f  m_position;      // Three dimensional coordinates (x, y, z)
	Vec3f  m_velocity;      // The velocity vector (vx, vy, vz)
	flt    m_yaw;           // horizontal rotation
	flt    m_mass;
	Shape  m_shape;

	bool    m_affected_by_gravity;
	bool    m_enabled_movement;
	uint8_t m_collision_group;
};

class RigidBodyController
{
// protected members
protected:
	Entity *m_entity;

// protected methods
protected:
	bool isValid() const;

// public methods
public:

	// interface methods
	void setup(Entity *parent);

	void tick_movement_intent(flt delta_time);

	void tick_dynamic_collision(flt delta_time);

	void tick_static_collision(flt delta_time);
};