#pragma once

#include "stdafx.h"

#include "game/world.h"

struct Grid
{
	int m_clearance;
	vector<Entity*> m_entities;	
};

class GridMap
{
// protected typedef
protected:
	struct GridContainer
	{
		int m_salt;
		Grid m_grid;
		void clear() {
			m_grid.m_entities.clear();
		}
	};

// protected members
protected:
	vector<GridContainer> m_grids;
	Vec2<size_t> m_size = {0, 0};
	int m_salt = 0;
	World::GamePlayRange<Vec2i> m_range = {Vec2i{0}, Vec2i{0}};

// public methods
public:
	// common interface
	void setup(size_t w, size_t h);
	void setupWorld(World *world);
	void refresh();
	void refreshEntities(World *world);

	Grid *getGrid(size_t i, size_t j);
	Grid *getGrid(Vec2i p);
	void iterateGrids(function<void(Grid*)> do_sth);
	
};

inline Grid *GridMap::getGrid(size_t i, size_t j)
{
	size_t offset = i * m_size[1] + j;
	if (offset >= m_grids.size()) return nullptr;
	GridContainer &container = m_grids[offset];
	if (container.m_salt != m_salt) {
		container.m_salt = m_salt;
		container.clear();
	}
	return &container.m_grid;
}

inline Grid *GridMap::getGrid(Vec2i p)
{
	return getGrid(p[0], p[1]);
}

inline void GridMap::refresh()
{
	++m_salt;
}

inline void GridMap::iterateGrids(function<void(Grid*)> do_sth)
{
	for (auto &container : m_grids) {
		if (container.m_salt != m_salt) {
			container.clear();
			container.m_salt = m_salt;
		}
		do_sth(&container.m_grid);
	}
}