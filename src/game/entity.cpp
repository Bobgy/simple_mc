#include "stdafx.h"

#include <cstring>
#include <cmath>
#include <algorithm>
#include <cassert>

#include "utility/vec.h"
#include "game/entity.h"

using namespace std;

void Entity::setup(shared_ptr<EntityController> entity_controller)
{
	p_entity_controller = entity_controller;
	if (p_entity_controller != nullptr) p_entity_controller->setup(this);

	shared_ptr<ActorHuman> actor_human = make_shared<ActorHuman>();
	actor_human->setup(this);
	p_actor = actor_human;
}

void Entity::tick(flt delta_time)
{
	if (p_entity_controller) p_entity_controller->tick(delta_time);
	if (p_actor) p_actor->tick(delta_time);
	if (M) {
		p = p + v;
		v = v * RESISTANCE;
	}
}

//calculate the collision with a cube with its lower coordinates at x
//returns whether x is exactly on the block
bool Entity::collide_cube_vertically(Vec3i x){
	if (!test_circle_rectangle_intersect(p[0]-x[0],p[2]-x[2],r,1,1)) return false;
	// inelastic collision for top and bottom face
	if (in_range((flt)p[1], x[1]+0.5f, x[1]+1.0f, false, true)) { //collide with top face
		p[1] = x[1] + 1.f;
		if (v[1] <= 0.f) {
			v[1] = 0.f;
			return true;
		}
	} else if (in_range(p[1] + h, (flt)x[1], x[1]+0.5f, true, false)){ //collide with bottom face
		p[1] = x[1] - h;
		if (v[1] > 0) v[1] = 0;
	}
	return false;
}

//calculate the collision with a cube with its sides
void Entity::collide_cube_horizontally(const Vec3i x){
	flt len = seg_intersect((flt)x[1], x[1] + 1.f, (flt)p[1], p[1] + h);
	if (zero(len)) return; //not vertically intersecting
	Vec3f cx;
	cx = Vec3f(x) + 0.5f;
	if (zero(p[0] - cx[0]) && zero(p[2] - cx[2])) return; //the center coincide, cannot collide
	int a, b, dt;
	get_quadrant(p[0] - cx[0], p[2] - cx[2], a, dt);
	b = a == 0 ? 2 : 0; //collide along "a" axis;
	assert(a != b && (a == 0 || a == 2) && (b == 0 || b == 2));
	int sig = dt ? 1 : -1;
	if (in_range(p[b], x[b], x[b] + 1, true, true)) {
		if (in_range(p[a] - sig*r, cx[a], x[a] + dt, false, true)){
			p[a] = x[a] + dt + sig * r;
			if ((v[a] > 0) != dt) v[a] = 0;
		}
	} else { //collide with the corner
		Vec3f p_corner;
		p_corner = x;
		p_corner[a] += dt;
		p_corner[b] = p[b] < cx[b] ? x[b] : x[b] + 1.f;
		Vec3f rel = p_corner - p;
		// the corner
		if (!test_point_in_circle(rel[0], rel[2], r)) return;
		rel = rel.normalize();
		//force((len - r - 1.0)*rel.normalize()); //elastic
		p = p_corner - rel * r;
		if (v*rel > 0) v = v - (v*rel)* rel;
	}
}

//test intersection
bool Entity::intersect_cube(Vec3i x){
	if (!test_circle_rectangle_intersect(p[0] - x[0], p[2] - x[2], r, 1, 1)) return false;
	if (zero(seg_intersect(x[1], x[1] + 1, p[1], p[1] + h)))return false;
	return true;
}

void Actor::setup(Entity *parent)
{
	m_parent = parent;
}

void ActorHuman::setup(Entity *parent, flt arm_swing_ang_speed, flt side_walk_body_ang)
{
	Actor::setup(parent);

	arm_ang = 0.0f;
	arm_swing_speed = arm_swing_ang_speed;

	body_ang = 0.0f;
	body_max_ang = side_walk_body_ang;
}

void ActorHuman::tick(flt delta_time)
{
	if (m_parent == nullptr) return;
	
	EntityController *controller = m_parent->getController();
	if (controller == nullptr) return;

	const EntityController::MovementIntent &movement_intent = controller->getMovementIntent();
	if (movement_intent.isWalking()) {
		arm_ang += arm_swing_speed;
		if (arm_ang > 45.f) {
			arm_ang = 45.f;
			arm_swing_speed = -arm_swing_speed;
		}
		if (arm_ang < -45.f) {
			arm_ang = -45.f;
			arm_swing_speed = -arm_swing_speed;
		}
	} else {
		arm_ang *= 0.96f;
		if (sgn(arm_ang) == 0) arm_ang = 0.0f;
	}

	int32_t side_walk = sgn(movement_intent.walk_intent[1]);
	switch (side_walk) {
	case 0:
		body_ang *= 0.96f;
		if (sgn(body_ang) == 0) body_ang = 0.0f;
		break;
	case 1:
		body_ang = (body_ang + 0.2f * 45.f) / 1.2f;
		break;
	case -1:
		body_ang = (body_ang + 0.2f * -45.f) / 1.2f;
		break;
	default:
		assert(false && "side_walk is unexpected");
	}
}
