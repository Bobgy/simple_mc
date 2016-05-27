#ifndef _WORLD_H_
#define _WORLD_H_

#include <map>
#include <vector>
#include <stdafx.h>
#include <set>
#include <list>
#include <memory>

#include "game/block.h"
#include "game/entity.h"
#include "game/player.h"

using namespace std;

namespace scripts
{
	class ScriptLevel;
}

class World{
// protected members
protected:
	map<Vec3i, Block*> blocks;
	map<Vec3i, weak_ptr<Entity>> m_entity_map;

	vector<Block> block_list;
	set<block_type> ability;
	vector<shared_ptr<Entity>> entity_list;
	shared_ptr<Player> p_player;

	shared_ptr<scripts::ScriptLevel> m_script;

// protected methods;
protected:
	void init_ability();
	void refreshEntityMap();

// public members
public:
	//indicates whether this world is changed (requires regenerating tablelist)
	bool changed = false;

	// helper members
	struct GamePlayRange {
		Vec3i m_min;
		Vec3i m_max;
	} m_game_play_range;

	typedef map<Vec3i, Block*>::iterator MapBlockIterator;

// public methods
public:

	/*========= constructor and destructor =======*/
	World() = default;
	~World() = default;
	// read the world data from stage_file_path
	void readFromFile(string stage_file_path);
	// generate a world randomly by seed: seed and range: range
	void randomGenerate(int seed, int range);

	/*======== interface methods ========*/

	void clear();
	void setup(shared_ptr<scripts::ScriptLevel> level_script);
	void tick(flt delta_time);
	bool addPlayer(shared_ptr<Player> player);

	// returns its ID, fails when ID is negative
	int spawnEntity(shared_ptr<Entity> entity);
	
	/*========= getter and setter methods =======*/

	//get the block at (p[0],p[1],p[2]), NULL means AIR block
	Block* get_block(Vec3i p) const;

	// get entity with entity_id as its id
	// returns nullptr when invalid entity_id is given
	Entity *getEntity(int entity_id);
	const vector<shared_ptr<Entity>> &getEntityList() const;

	// iterate over the entity list and do something on them
	void iterateEntityList(function<void(Entity *)> do_sth);

	const map<Vec3i, weak_ptr<Entity>> &getEntityMap() const;

	// get the player
	Player *getPlayer();

	//returns the first block within radius r that is seen by an eye
	//at p looking at the direction vector dir
	//returns -1 in face to indicate no block is found
	BlockAndFace look_at_block(Vec3fd p, Vec3fd dir, double r) const;

	Vec3f getRandomPosition() const;

	/* block related methods*/

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
