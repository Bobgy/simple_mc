#include "stdafx.h"

#include <cstring>
#include <cmath>
#include <algorithm>
#include <cassert>

#include "utility/vec.h"
#include "game/entity.h"
#include "game/components/rigidbody_motion_controller.h"

using namespace std;

void Entity::setup(
	shared_ptr<EntityController> entity_controller,
	shared_ptr<RigidBodyMotionController> rigidbody_motion_controller)
{
	m_entity_controller = entity_controller;
	if (m_entity_controller) m_entity_controller->setup(this);
	m_rigid_body_motion_controller = rigidbody_motion_controller;
	if (!m_rigid_body_motion_controller) {
		//m_rigid_body_motion_controller = make_shared<RigidBodyMotionController>();
		m_rigid_body_motion_controller = make_shared<HumanMotionController>();
		assert(m_rigid_body_motion_controller);
	}
	m_rigid_body_motion_controller->setup(&m_rigid_body);

	shared_ptr<RigidBodyController> rigid_body_controller = make_shared<RigidBodyController>();
	rigid_body_controller->setup(this, m_rigid_body_motion_controller.get());
	m_rigid_body_controller = rigid_body_controller;

	shared_ptr<ActorHuman> actor_human = make_shared<ActorHuman>();
	actor_human->setup(this);
	m_actor = actor_human;

	m_priority = Priority{0, 0, 0};
	m_temporary_priority = {{0, 0, 0}, 0};
	m_nav_force = Vec2f::ZERO();
}

void Entity::tick(flt delta_time)
{
	if (m_actor) m_actor->tick(delta_time);
	m_nav_force *= k_nav_force_deteriorate_rate;
}

void Entity::tick_controller(flt delta_time)
{
	if (m_entity_controller) {
		m_entity_controller->tick(delta_time);
	}
}

//calculate the collision with a cube with its lower coordinates at x
//returns whether x is exactly on the block
bool Entity::collide_cube_vertically(Vec3i x){
	bool is_circle_rectangle_intersected = test_circle_rectangle_intersect(
		m_rigid_body.m_position[0] - x[0],
		m_rigid_body.m_position[2] - x[2],
		ASSERT_PTR(m_rigid_body.m_shape.getCylinder())->r,
		1, 1);
	if (!is_circle_rectangle_intersected) return false;
	// inelastic collision for top and bottom face
	if (in_range((flt)m_rigid_body.m_position[1], x[1]+0.5f, x[1]+1.0f, false, true)) { //collide with top face
		m_rigid_body.m_position[1] = x[1] + 1.f;
		if (m_rigid_body.m_velocity[1] <= 0.f) {
			m_rigid_body.m_velocity[1] = 0.f;
			return true;
		}
	} else if (in_range(m_rigid_body.m_position[1] + ASSERT_PTR(m_rigid_body.m_shape.getCylinder())->h, (flt)x[1], x[1]+0.5f, true, false)){ //collide with bottom face
		m_rigid_body.m_position[1] = x[1] - ASSERT_PTR(m_rigid_body.m_shape.getCylinder())->h;
		if (m_rigid_body.m_velocity[1] > 0) m_rigid_body.m_velocity[1] = 0;
	}
	return false;
}

//calculate the collision with a cube with its sides
void Entity::collide_cube_horizontally(const Vec3i x){
	flt len = seg_intersect((flt)x[1], x[1] + 1.f, (flt)m_rigid_body.m_position[1], m_rigid_body.m_position[1] + ASSERT_PTR(m_rigid_body.m_shape.getCylinder())->h);
	if (zero(len)) return; //not vertically intersecting
	Vec3f cx;
	cx = Vec3f(x) + 0.5f;
	if (zero(m_rigid_body.m_position[0] - cx[0]) && zero(m_rigid_body.m_position[2] - cx[2])) return; //the center coincide, cannot collide
	int a, b, dt;
	get_quadrant(m_rigid_body.m_position[0] - cx[0], m_rigid_body.m_position[2] - cx[2], a, dt);
	b = a == 0 ? 2 : 0; //collide along "a" axis;
	assert(a != b && (a == 0 || a == 2) && (b == 0 || b == 2));
	int sig = dt ? 1 : -1;
	if (in_range(m_rigid_body.m_position[b], x[b], x[b] + 1, true, true)) {
		if (in_range(m_rigid_body.m_position[a] - sig*ASSERT_PTR(m_rigid_body.m_shape.getCylinder())->r, cx[a], x[a] + dt, false, true)){
			m_rigid_body.m_position[a] = x[a] + dt + sig * ASSERT_PTR(m_rigid_body.m_shape.getCylinder())->r;
			if ((m_rigid_body.m_velocity[a] > 0) != dt) m_rigid_body.m_velocity[a] = 0;
		}
	} else { //collide with the corner
		Vec3f p_corner;
		p_corner = x;
		p_corner[a] += dt;
		p_corner[b] = m_rigid_body.m_position[b] < cx[b] ? x[b] : x[b] + 1.f;
		Vec3f rel = p_corner - m_rigid_body.m_position;
		// the corner
		if (!test_point_in_circle(rel[0], rel[2], ASSERT_PTR(m_rigid_body.m_shape.getCylinder())->r)) return;
		rel = rel.normalize();
		//force((len - r - 1.0)*rel.normalize()); //elastic
		m_rigid_body.m_position = p_corner - rel * ASSERT_PTR(m_rigid_body.m_shape.getCylinder())->r;
		if (m_rigid_body.m_velocity*rel > 0) m_rigid_body.m_velocity = m_rigid_body.m_velocity - (m_rigid_body.m_velocity*rel)* rel;
	}
}

//test intersection
bool Entity::intersect_cube(Vec3i x){
	if (!test_circle_rectangle_intersect(m_rigid_body.m_position[0] - x[0], m_rigid_body.m_position[2] - x[2], ASSERT_PTR(m_rigid_body.m_shape.getCylinder())->r, 1, 1)) return false;
	if (zero(seg_intersect(x[1], x[1] + 1, m_rigid_body.m_position[1], m_rigid_body.m_position[1] + ASSERT_PTR(m_rigid_body.m_shape.getCylinder())->h)))return false;
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

	const MovementIntent &movement_intent = controller->getMovementIntent();
	if (movement_intent.isWalking(m_parent->m_rigid_body.m_yaw)) {
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

void Entity::setPriority(Priority priority)
{
	m_priority = priority;
}

void Entity::setTemporaryPriority(TemporaryPriority temporary_priority)
{
	uint32_t current_tick = CurrentGame()->getTickCount();
	RETURN_AND_WARN_IF(temporary_priority.m_expire_tick < current_tick);
	if (m_temporary_priority.m_expire_tick < current_tick ||
		temporary_priority.m_priority <= m_temporary_priority.m_priority) {
		m_temporary_priority = temporary_priority;
	}
}

const Entity::Priority & Entity::getPriority() const
{
	if (m_temporary_priority.m_expire_tick >= CurrentGame()->getTickCount()) {
		return m_temporary_priority.m_priority;
	}
	return m_priority;
}