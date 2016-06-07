#include "stdafx.h"

#include <map>
#include <cassert>
#include <iostream>
#include <fstream>
#include <random>

#include "utility/vec.h"
#include "utility/config.h"

#include "game/game.h"
#include "game/block.h"
#include "game/world.h"
#include "game/rigid_body.h"
#include "game/entity.h"
#include "game/gridmap.h"
#include "game/components/ticker.h"

#include "scripts/script.h"

using namespace std;

static vector<pair<Entity::Priority, Entity *>> g_priority_entity;
static vector<Entity*> g_priority_entity_list;
static TickerBFS<Entity> g_bfs_ticker;

void World::tick(flt delta_time)
{
	// tick level script
	m_script->tick(delta_time);

	m_grid_map->refreshEntities(this);

	// tick EntityController logic
	if (bPriorityEnabled) {
		g_priority_entity.clear();
		for (auto &entity : entity_list) {
			g_priority_entity.push_back(make_pair(entity->getPriority(), entity.get()));
		}
		sort(g_priority_entity.begin(), g_priority_entity.end());
		g_priority_entity_list.clear();
		for (auto &pr : g_priority_entity) {
			g_priority_entity_list.push_back(pr.second);
		}
		g_bfs_ticker.setup(&g_priority_entity_list);
		g_bfs_ticker.tick(delta_time);
	} else {
		for (auto &entity : entity_list) {
			entity->tick_controller(delta_time);
		}
	}

#ifdef _USE_ENTITY_MAP
	refreshEntityMap();

	for (auto &entity : entity_list) {
		entity->tick(delta_time);
		RigidBodyController *controller = entity->getRigidBodyController();
		if (controller != nullptr) {
			controller->tick_dynamic_collision(delta_time);
			m_entity_map.insert(make_pair(entity->m_rigid_body.getCenterCoord(), entity));
		}
	}
#else
	for (auto &entity : entity_list) {
		entity->tick(delta_time);
		RigidBodyController *controller = entity->getRigidBodyController();
		if (controller != nullptr) {
			controller->tick_dynamic_collision(delta_time);
		}
	}
#endif

	// tick RigidBodyController movement_intent
	for (auto &entity : entity_list) {
		RigidBodyController *controller = entity->getRigidBodyController();
		if (controller != nullptr) {
			controller->tick_movement_intent(delta_time);
			controller->tick_physical_simulation(delta_time);
		}
	}

	for (auto &entity : entity_list) {
		RigidBodyController *controller = entity->getRigidBodyController();
		if (controller != nullptr) controller->tick_static_collision(delta_time);
	}
}

bool World::addPlayer(shared_ptr<Player> player)
{
	if (p_player) return false;
	p_player = player;
	p_player->setup();
	return true;
}

size_t World::spawnEntity(shared_ptr<Entity> entity) {
	entity_list.push_back(entity);
	size_t id = entity_list.size() - 1;
	entity->setID(id);
	return id;
}

void World::clear()
{
	p_player.reset();
	blocks.clear();
	block_list.clear();
	ability.clear();
	entity_list.clear();
	m_entity_map.clear();
	m_grid_map.reset();
}

void World::setup(shared_ptr<scripts::ScriptLevel> level_script)
{
	if (!level_script) return;

	m_script = level_script;
	m_script->setup_level();

	auto grid_map = make_shared<GridMap>();
	if (grid_map) {
		m_grid_map = grid_map;
		m_grid_map->setupWorld(this);
	} else {
		LOG_ERROR(__FUNCTION__, "grid_map failed initialization.\n");
	}
}

//get the block at (p[0],p[1],p[2]), NULL means AIR block
Block* World::get_block(Vec3i p) const {
	auto it = blocks.find(p);
	if (it != blocks.end()) return it->second;
	return NULL;
}

Entity *World::getEntity(int entity_id)
{
	if (entity_id >= 0 && (size_t)entity_id < entity_list.size()) {
		return entity_list[entity_id].get();
	}
	return nullptr;
}

const vector<shared_ptr<Entity>>& World::getEntityList() const
{
	return entity_list;
}

void World::iterateEntityList(function<void(Entity*)> do_sth)
{
	for (auto &entity_ptr : entity_list) {
		if (entity_ptr) {
			do_sth(entity_ptr.get());
		}
	}
}

const multimap<Vec3i, weak_ptr<Entity>>& World::getEntityMap() const
{
	return m_entity_map;
}

GridMap *World::getGridMap()
{
	return m_grid_map.get();
}

weak_ptr<GridMap> World::getGridMapWeakPtr() const
{
	return m_grid_map;
}

Player *World::getPlayer()
{
	if (p_player) return p_player.get();
	return nullptr;
}

//returns the first block within radius r that is seen by an eye
//at p looking at the direction vector dir
//returns -1 in face to indicate no block is found

void World::init_ability()
{
	ability.clear();
	ability.insert(TREASURE);
}

void World::refreshEntityMap()
{
	m_entity_map.clear();
	/*
	for (auto &entity : entity_list) {
		m_entity_map[round(entity->m_rigid_body.m_position)] = entity;
	}
	*/
}

BlockAndFace World::look_at_block(Vec3fd p, Vec3fd dir, double r) const {
	static int sign[3], i;
	for (i = 0; i < 3; ++i)
		sign[i] = sgn(dir[i]);
	assert(sign[0] || sign[1] || sign[2]);
	Vec3fd now(p);
	Vec3i p_block;
	int MAX_COUNT = 200;
	while(MAX_COUNT--){
		int axis = -1;
		double next_time = 1e20; //infinite
		//find the next intersection point with an integer face
		for (i = 0; i < 3; ++i) {
			if (sign[i] == 0) continue;
			int ni = next_int(now[i], sign[i]);
			double t = (ni - now[i]) / dir[i];
			if (t < 0) {
				next_time = t;
			}
			assert(t >= 0);
			if (t < next_time) {
				next_time = t;
				axis = i;
			}
		}
		assert(~axis);
		now = now + (next_time + 1e-10) * dir;
		if (sqr(now - p) > r*r) break;
		p_block = floor(now);
		auto it = blocks.find(p_block);
		if (it != blocks.end() && it->second->test_intersection(p, dir)){
			return make_pair(p_block, FACE_AXIS[axis][sign[axis]<0]);
		}
	}
	assert(MAX_COUNT >= 0);
	return make_pair(p_block, -1); //-1 means not found
}

Vec3f World::getRandomPosition() const
{
	Vec3f ret;
	for (size_t i = 0; i < 3; ++i) {
		uniform_int_distribution<> dis(m_game_play_range.m_min[i], m_game_play_range.m_max[i]);
		ret[i] = dis(k_pseudo_gen);
	}
	return ret;
}

void World::readFromFile(string stage_file_path) {
	init_ability();
	for (int i = 0; i < 10; i++)
		block_list.push_back(Block(block_type(i)));
	if (!read_from_file(stage_file_path)) {
		//fail to construct the world from file, generate it randomly
		cerr << "Failed to read from file " << stage_file_path.c_str() << endl;
		return;
	}
	changed = true;
}

void World::randomGenerate(int seed, int range, flt p) {
	init_ability();
	srand(seed);
	for (int i = 0; i < 10; i++) block_list.push_back(Block(block_type(i)));
	for (int i = -range; i <= range; ++i)
		for (int j = -range; j <= range; ++j)
			for (int k = 0; k <= 3; ++k) {
				if (abs(i) == range || abs(j) == range || k == 0 || (rand() % 100 >= p)) {
					blocks[Vec3i{i, k, j}] = &block_list[ENDSTONE];
				} else break;
			}
	changed = true;
}

//place a block at p of type tp
bool World::place_block(BlockAndFace p, block_type tp){
	Vec3i pos = p.first + FACE[p.second];
	Entity &observer = *p_player->getEntity();
	// TODO: test intersection with all agents
	if (get_block(pos) == NULL && !observer.intersect_cube(pos)){
		blocks[pos] = &block_list[tp];
		changed = true;
		//cout << tp << endl;
		return true;
	}
	return false;
}

bool World::destroy_block(Vec3i p){
	Block * now = get_block(p);
	// Here we add the "ability" to check whether the observer is able to destroy that kind of block 
	if ((now != NULL)) { //&&(ability.count(now->get_block_type())==1 || bCreative)) {
		if (now->get_block_type() == TREASURE)
		{
			/*
			Here the observer destroys the goal--TREASURE, and finishes the game.
			Something must be done to end the game.
			*/
		}
		blocks.erase(p);
		changed = true;
		return true;
	}
	return false;
}

void World::print()
{
	ofstream f("stage/last_save.txt");
	static map<Vec3i, Block*>::const_iterator it;
	for (it = blocks.begin(); it != blocks.end(); ++it)
	{
		Vec3i point = it->first;
		Block* type = it->second;
		f << point[0] << " " << point[1] << " " << point[2] << " " << type->get_block_type() << endl;

	}
	f.close();
}

bool World::read_from_file(string name)
{
	int x, y, z, type;
	blocks.clear();
	ifstream f(name);
	if (f.rdstate() & f.failbit) return false;
	f >> x >> y >> z >> type;
	while (!f.eof()) {
		Vec3i pos{x, y, z};
		blocks[pos] = &block_list[block_type(type)];
		f >> x >> y >> z >> type;
	}
	return true;
}

void World::add(int type)
{
	ability.insert(block_type(type));
}