#ifndef _WORLD_H_
#define _WORLD_H_

#include <map>
#include <vector>
#include <stdafx.h>
#include <core/block.h>
#include <core/entity.h>
#include <set>

using namespace std;

class World{

private:

	map<Vec3i, Block*> blocks;
	vector<Block> block_list;
	set<block_type> ability;
	void init_ability();

public:

	//indicates whether this world is changed (requires regenerating tablelist)
	bool changed;

	typedef map<Vec3i, Block*>::iterator MapBlockIterator;

	//get the block at (p[0],p[1],p[2]), NULL means AIR block
	Block* get_block(Vec3i p) const;

	//returns the first block within radius r that is seen by an eye
	//at p looking at the direction vector dir
	//returns -1 in face to indicate no block is found
	BlockAndFace look_at_block(Vec3<double> p, Vec3<double> dir, double r) const;

	//generate a world by a stage file
	World(string stage_file_path);

	//generate a world by random seed: seed
	World(int seed, int range);

	//place a block at p of type tp
	bool place_block(BlockAndFace p, block_type tp);

	//destroy a block at p
	bool destroy_block(Vec3i p);

	map<Vec3i, Block*>::const_iterator begin() const {
		return blocks.begin();
	}
	map<Vec3i, Block*>::const_iterator end() const {
		return blocks.end();
	}
	map<Vec3i, Block*>::const_iterator find(Vec3i p) const {
		return blocks.find(p);
	}

	void print();

	//Read the world from a file, returns true when successful, false otherwise.
	bool read_from_file(string name);

	void add(int type);
};
#endif
