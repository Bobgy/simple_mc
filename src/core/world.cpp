#include <map>
#include <cassert>
#include <iostream>
#include <fstream>

#include <core/vec.h>
#include <core/block.h>
#include <core/world.h>
#include <utility/config.h>

using namespace std;

extern Entity observer;

//get the block at (p[0],p[1],p[2]), NULL means AIR block
Block* World::get_block(Vec3i p) const {
	auto it = blocks.find(p);
	if (it != blocks.end()) return it->second;
	return NULL;
}

//returns the first block within radius r that is seen by an eye
//at p looking at the direction vector dir
//returns -1 in face to indicate no block is found

void World::init_ability()
{
	ability.clear();
	ability.insert(TREASURE);
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
		//find the next intersection point with an interger face
		for (i = 0; i < 3; ++i) {
			if (sign[i] == 0) continue;
			int ni = next_int(now[i], sign[i]);
			double t = (ni - now[i]) / dir[i];
			assert(t >= 0);
			if (t < next_time){
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

//generate a world by file in path stage_file_path
World::World(string stage_file_path):changed(false){
	init_ability();
	for (int i = 0; i < 10; i++)
		block_list.push_back(Block(block_type(i)));
	if (!read_from_file(stage_file_path)) {
		//fail to construct the world from file, generate it randomly
		cerr << "Failed to read from file " << stage_file_path.c_str() << endl;
	}
}

//generate a world by random seed: seed
World::World(int seed, int range):changed(false){
	init_ability();
	srand(seed);
	for (int i = 0; i < 10; i++) block_list.push_back(Block(block_type(i)));
	for (int i = -range; i <= range; ++i)
		for (int j = -range; j <= range; ++j)
			for (int k = 0; k <= 3; ++k){
				int t = (1 << (k + 2)) - 1;
				if (k == 0 || (rand()&t) == t) blocks[Vec3i(i, k, j)] = &block_list[DIRT];
				else break;
			}
}

//place a block at p of type tp
bool World::place_block(BlockAndFace p, block_type tp){
	Vec3i pos = p.first + FACE[p.second];
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
	block_type bt;
	blocks.clear();
	ifstream f(name);
	if (f.rdstate() & f.failbit) return false;
	f >> x >> y >> z >> type;
	while (!f.eof())
	{
		Vec3i pos(x, y, z);
		blocks[pos] = &block_list[block_type(type)];
		f >> x >> y >> z >> type;
	}
	return true;
}

void World::add(int type)
{
	ability.insert(block_type(type));
}