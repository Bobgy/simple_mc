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
	Vec3f  m_acceleration;  // The acceleration vector (ax, ay, az)
	flt    m_yaw;           // horizontal rotation
	flt    m_pitch;         // vertical rotation
	flt    m_mass;
	flt    m_mass_inv;
	Shape  m_shape;

	bool    m_affected_by_gravity;
	bool    m_enabled_movement;
	uint8_t m_collision_group;
	Vec3f getHorizontalFacingVector() const {
		return horizontal_facing_vector(m_yaw);
	}
	Vec3f getFacingVector() const {
		return facing_vector(m_yaw, m_pitch);
	}
	Vec3i getCenterCoord() const {
		return round(m_position);
	}
	flt intersect(const RigidBody &r) const;
	void force(Vec3f F) { if (m_enabled_movement) m_velocity += F * (1.0f / m_mass); }
	void collision_force(Vec3f d, flt F) {
		if (m_enabled_movement) {
			d = d.normalize();
			flt len = m_velocity * d;
			if (len < 2.f) {
				force(d * min(F, (2.f - len) * m_mass));
			}
		}
	}
	void give_velocity(Vec3f _p, flt len) {
		if (m_enabled_movement) {
			Vec3f p_norm, v_p;
			p_norm = _p.normalize();
			v_p = (m_velocity * p_norm) * p_norm;
			m_velocity = (m_velocity - v_p) + ((v_p * 4.0f + _p * len) * (1.0f / 5.f));
		}
	}
};

class RigidBodyMotionController;
class RigidBodyController
{
// protected members
protected:
	Entity *m_entity;
	RigidBodyMotionController *m_motion_controller;

// protected methods
protected:
	bool isValid() const;

// public methods
public:

	// interface methods
	void setup(Entity *parent, RigidBodyMotionController *motion_controller);

	void tick_movement_intent(flt delta_time);

	void tick_physical_simulation(flt delta_time);

	void tick_dynamic_collision(flt delta_time);

	void tick_static_collision(flt delta_time);
};