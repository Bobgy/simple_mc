#ifndef _ENTITY_H
#define _ENTITY_H

#include <cstring>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <memory>

#include "utility/vec.h"
#include "utility/geo.h"
#include "game/entity_controller.h"

class Actor;

class Entity {
// public typedef
public:
	struct RenderConfig {
		bool is_visible = true;
		bool has_shadow = true;
	};

// protected members
protected:
	Vec3f p;      //Three dimensional coordinates (x, y, z)
	Vec3f v;      //The speed vector (vx, vy, vz)
	flt mass_inv; //The inverse of mass: mass_inv = 1 / mass
	bool G;       //Is this entity affected by gravity?
	bool M;       //Can this entity move?
	flt r, h;     //Collision shape as a cylinder with radius r and height h.

	Rotation rot; //Rotation of this entity

	// the controller, can be either an AI controller or a Player controller
	shared_ptr<EntityController> p_entity_controller;

	// the actor, may be null
	shared_ptr<Actor> p_actor;

// public members
public:
	RenderConfig render_config;

// protected methods
protected:
	inline bool zero(flt x) { return fabs(x) < EPS_COLLIDE; }
	inline int sgn(flt x) { return fabs(x) < EPS_COLLIDE ? 0 : (x > 0 ? 1 : -1); }
	inline bool in_block(flt x, int u) { return x >= u && x <= u + 1; }

// public methods
public:
	// constructor
	Entity(
		Vec3f p, Vec3f v,
		flt r, flt h,
		flt mass_inv,
		bool G = 1, bool M = 1
	):
		p(p), v(v),
		mass_inv(mass_inv),
		G(G), M(M),
		r(r), h(h),
		rot(0, 0),
		p_entity_controller(nullptr)
	{
		// do nothing
	}

	bool intersect_cube(Vec3i x);

	bool on_ground;  //Is this entity on the ground?

	operator Vec3f() const { return p; }
	Vec3f get_pos() const { return p; }
	flt getRadius() const { return r; }
	flt getHeight() const {	return h; }
	Vec3f get_velocity() const { return v; }
	EntityController *getController() {	return p_entity_controller.get(); }
	Actor *getActor() { return p_actor.get(); }
	const Actor *getActor() const { return p_actor.get(); }
	const Rotation *getRotation() const { return &rot; }
	void setRotation(flt h_rot, flt v_rot) { rot.setRotation(h_rot, v_rot);	}
	void setRotation(Vec2f rotation) { rot.setRotation(rotation); }
	flt operator[](size_t id) const { return p[id]; }
	//move to (p[0],p[1],p[2]) directly
	void move(Vec3f _p) { if (M) p = _p; }

	void give_velocity(Vec3f _p, flt len) {
		if (M) {
			Vec3f p_norm, v_p;
			p_norm = _p.normalize();
			v_p = (v * p_norm) * p_norm;
			v = (v - v_p) + ((v_p * 3.0f + _p * len) * 0.25f);
		}
	}
	void be_slowed(flt resistance) { if (M) v = v * resistance; }

	//be given an force of _F
	void force(Vec3f _F) { if (M) v = v + _F * (mass_inv * CLOCK_T); }

	//fall because of gravity
	void fall() { if (M && G) v[1] -= GRAVITY * CLOCK_T; }

	//calculate the collision with a cube with its lower coordinates at x
	//returns whether x is exactly on the block
	bool collide_cube_vertically(const Vec3i x);

	void collide_cube_horizontally(const Vec3i x);

	/*============= interface methods =============*/
	
	// setup components
	void setup(shared_ptr<EntityController> entity_controller);

	// tick to simulate the movement in time seconds
	void tick(flt delta_time);
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