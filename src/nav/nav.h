#pragma once

#include "stdafx.h"

#include <map>

class World;
class GridMap;
class Pathfinder;

class NavigationManager
{
// public typedef
public:
	struct PathHandle
	{
		uint32_t m_index = ~0U;
	};
	struct Path
	{
		shared_ptr<vector<Vec2f>> m_road_points;
	};
// public const static members
	const static PathHandle k_invalid_handle;
// protected members
protected:
	map<uint32_t, Path> m_paths;
	uint32_t m_next_index = 0;
	weak_ptr<GridMap> m_gridmap;

// protected methods
	uint32_t genIndex();

// public methods
public:
	/* common interface */
	void setup(World *world);
	PathHandle queryPath(Vec2f start_point, Vec2f end_point, flt radius, bool consider_entities = false);
	void releaseHandle(PathHandle handle);
	Path *getPath(PathHandle handle);
};

class Pathfinder
{
// public methods
public:
	// constructor and destructor
	virtual ~Pathfinder() = 0;
	virtual shared_ptr<vector<Vec2f>> queryPath(Vec2f start_point, Vec2f end_point, flt radius, bool consider_entities = false) = 0;
};