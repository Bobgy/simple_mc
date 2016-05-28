#include "stdafx.h"

#include "gridmap.h"

#include "game/world.h"

void GridMap::clear()
{
	m_salt = 0;
	m_grids.clear();
}

void GridMap::setup(size_t w, size_t h)
{
	m_size = {w, h};
	m_grids.clear();
	m_grids.resize(m_size[0]*m_size[1]);
}

void GridMap::setupWorld(World * world)
{
	RETURN_AND_WARN_IF(world == nullptr);
	refresh();

	World::GamePlayRange<Vec3i> &world_range = world->m_game_play_range;
	m_range.m_min = horizontal_projection(world_range.m_min);
	m_range.m_max = horizontal_projection(world_range.m_max);

	setup(m_range.m_max[0] - m_range.m_min[0] + 1, m_range.m_max[1] - m_range.m_min[1] + 1);
	iterateGrids([](Grid *grid){
		grid->m_clearance = 1;
	});
	for (auto it = world->blocks_begin(); it != world->blocks_end(); ++it) {
		Vec3i pos = it->first;
		Block *block = it->second;
		if (world_range.isInside(pos)) {
			Vec2i p2 = horizontal_projection(pos);
			p2 -= m_range.m_min;
			Grid *grid = getGrid(p2);
			assert(grid);
			grid->m_clearance = 0;
		}
	}
}

void GridMap::refreshEntities(World *world)
{
	RETURN_AND_WARN_IF(world == nullptr);

	refresh();
	auto entity_list = world->getEntityList();
	for (auto &entity_ptr: entity_list) {
		if (entity_ptr) {
			Vec2i p2 = horizontal_projection(entity_ptr->m_rigid_body.getCenterCoord());
			Grid *grid = getGridByWorldCoord(p2);
			if (grid == nullptr) {
				LOG_ERROR(__FUNCTION__, "refreshing entity which is at (%d, %d).\n", p2[0], p2[1]);
			} else {
				grid->m_entities.push_back(entity_ptr.get());
			}
		}
	}
}
