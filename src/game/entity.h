#ifndef _ENTITY_H
#define _ENTITY_H

#include "stdafx.h"

#include <cstring>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <memory>

#include "utility/vec.h"
#include "utility/geo.h"
#include "game/entity_controller.h"
#include "game/rigid_body.h"

class Actor;

class Entity {
// public typedef
public:
	struct RenderConfig {
		bool is_visible = true;
		bool has_shadow = true;
	};
	struct Priority {
		// lower value means higher priority
		uint16_t m_tie_breaker;
		uint16_t m_propagated_steps;
		uint32_t m_context;

		uint32_t get() const {
			return ((uint64_t)m_context << 32) | ((uint32_t)m_propagated_steps << 16) | m_tie_breaker;
		}
		bool operator < (Priority r) const {
			return get() < r.get();
		}
		bool operator <= (Priority r) const {
			return get() <= r.get();
		}
	};
	struct TemporaryPriority {
		Priority m_priority;
		uint32_t m_expire_tick;
	};

// protected members
protected:
	size_t m_id;
	bool has_collision = true;
	bool should_tick = true;

	// the controller, can be either an AI controller or a Player controller
	shared_ptr<EntityController> m_entity_controller;
	shared_ptr<RigidBodyController> m_rigid_body_controller;
	shared_ptr<RigidBodyMotionController> m_rigid_body_motion_controller;

	// the actor, may be null
	shared_ptr<Actor> m_actor;

	Priority m_priority;
	TemporaryPriority m_temporary_priority;

// public members
public:
	RenderConfig render_config;
	RigidBody m_rigid_body;
	bool on_ground;
	Vec2f m_nav_force;

// protected methods
protected:
	inline bool zero(flt x) { return fabs(x) < EPS_COLLIDE; }
	inline int sgn(flt x) { return fabs(x) < EPS_COLLIDE ? 0 : (x > 0 ? 1 : -1); }
	inline bool in_block(flt x, int u) { return x >= u && x <= u + 1; }

// public methods
public:
	/*======== constructor and destructor =========*/
	Entity(
		Vec3f p, Vec3f v,
		flt r, flt h,
		flt mass_inv,
		bool G = 1, bool M = 1
	):
		m_entity_controller(nullptr)
	{
		m_rigid_body.m_position = p;
		m_rigid_body.m_velocity = v;
		m_rigid_body.m_yaw = 0.0f;
		m_rigid_body.m_pitch = 0.0f;

		m_rigid_body.m_shape.setCylinder(r, h);
		m_rigid_body.m_mass = 1.0f / mass_inv;
		
		m_rigid_body.m_affected_by_gravity = G;
		m_rigid_body.m_enabled_movement = M;
	}

	/*****************************************************\
	|*============ getter and setter methods ============*|
	\*****************************************************/
	operator Vec3f() const { return m_rigid_body.m_position; }
	Vec3f get_pos() const { return m_rigid_body.m_position; }
	flt getRadius() const { return ASSERT_PTR(m_rigid_body.m_shape.getCylinder())->r; }
	flt getHeight() const {	return ASSERT_PTR(m_rigid_body.m_shape.getCylinder())->h; }
	Vec3f getVelocity() const { return m_rigid_body.m_velocity; }
	EntityController *getController() {	return m_entity_controller.get(); }
	Actor *getActor() { return m_actor.get(); }
	const Actor *getActor() const { return m_actor.get(); }
	RigidBodyController *getRigidBodyController() { return m_rigid_body_controller.get(); };
	const RigidBodyController *getRigidBodyController() const { return m_rigid_body_controller.get(); }
	flt operator[](size_t id) const { return m_rigid_body.m_position[id]; }
	void setPriority(Priority priority);
	void setTemporaryPriority(TemporaryPriority priority);
	const Priority &getPriority() const;
	uint32_t getTemporaryExpireTick() const;
	void setID(size_t id) { m_id = id; }
	size_t getID() const { return m_id; }
	void setCollision(bool state) { has_collision = state; }
	bool isCollisionOn() const { return has_collision; }
	void setTickState(bool state) { should_tick = state; }
	bool isTicking() const { return should_tick; }

	/*******************************************\
	|*============ physics methods ============*|
	\*******************************************/
	//move to (p[0],p[1],p[2]) directly
	void move(Vec3f _p) { if (m_rigid_body.m_enabled_movement) m_rigid_body.m_position = _p; }
	
	/***********************************************\
	|*============ geometry utilities =============*|
	\***********************************************/
	bool intersect_cube(Vec3i x);

	//calculate the collision with a cube with its lower coordinates at x
	//returns whether x is exactly on the block
	bool collide_cube_vertically(const Vec3i x);
	void collide_cube_horizontally(const Vec3i x);
	
	/***********************************************\
	|*============= interface methods =============*|
	\***********************************************/
	// setup components
	void setup(
		shared_ptr<EntityController> entity_controller,
		shared_ptr<RigidBodyMotionController> rigidbody_motion_controller = nullptr);

	// tick to simulate the movement in time seconds
	void tick(flt delta_time);
	void tick_controller(flt delta_time);
};

// actor is irrelevant to game logic making it purely relevant to the game's display
class Actor
{
// protected members
protected:
	// the parent of this actor, may be null
	Entity *m_parent = nullptr;

// public methods
public:
	/*======== constructor and destructor =========*/
	Actor() {};
	virtual ~Actor() {};

	/*============= interface methods =============*/
	void setup(Entity *parent);
	virtual void tick(flt delta_time) = 0;
};

// a human actor
class ActorHuman : public Actor
{
// protected members
protected:
	flt arm_ang = 0.0f;      // arm angle
	flt arm_swing_speed = 0.0f;    // arm swing speed

	flt body_ang = 0.0f;     // body rotate angle
	flt body_max_ang = 0.0f; // body rotate max angle

// public methods
public:
	/*======== constructor and destructor =========*/
	ActorHuman() = default;
	virtual ~ActorHuman() = default;

	/*=============== getter methods ==============*/
	flt getArmAng() const { return arm_ang; }
	flt getArmSwingSpeed() const { return arm_swing_speed; }
	flt getBodyAng() const { return body_ang; }
	flt getBodyMaxAng() const { return body_max_ang; }

	/*============= interface methods =============*/
	void setup(Entity *parent, flt arm_swing_ang_speed = 4.33f, flt side_walk_body_max_ang = 45.0f);
	virtual void tick(flt delta_time);
};

#endif