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
	void clear();
	void setup(size_t w, size_t h);
	void setupWorld(World *world);
	void refresh();
	void refreshEntities(World *world);

	// getter and setter
	const World::GamePlayRange<Vec2i> *getRange() const { return &m_range; }
	Vec2ui getSize() const { return m_size; }
	Grid *getGrid(size_t i, size_t j);
	Grid *getGrid(Vec2i p);
	Grid *getGridByWorldCoord(Vec2i p);
	Grid *getGridDirectly(size_t p);
	void iterateGrids(function<void(Grid*)> do_sth);
	void iterateGridsInRange(Vec2i low, Vec2i high, function<void(Vec2i, Grid*)> do_sth);
	
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

inline Grid *GridMap::getGridByWorldCoord(Vec2i p)
{
	return getGrid(p - m_range.m_min);
}

inline Grid *GridMap::getGridDirectly(size_t p)
{
	size_t offset = p;
	if (offset >= m_grids.size()) return nullptr;
	GridContainer &container = m_grids[offset];
	return &container.m_grid;
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

inline void GridMap::iterateGridsInRange(Vec2i low, Vec2i high, function<void(Vec2i, Grid*)> do_sth)
{
	low -= m_range.m_min;
	high -= m_range.m_min;
	low[0] = max(0, low[0]);
	low[1] = max(0, low[1]);
	high[0] = min((int)m_size[0] - 1, high[0]);
	high[1] = min((int)m_size[1] - 1, high[1]);
	for (int i = low[0]; i <= high[0]; ++i) {
		for (int j = low[1]; j <= high[1]; ++j) {
			Grid *grid = getGrid(i, j);
			if (grid) {
				do_sth(Vec2i{i, j} + m_range.m_min, grid);
			} else {
				LOG_WARNING(__FUNCTION__, "iterating over grid (%u, %u) which is invalid.\n", i, j);
			}
		}
	}
}
