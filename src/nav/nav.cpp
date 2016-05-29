#include "stdafx.h"

#include "nav.h"

#include "game/world.h"
#include "game/gridmap.h"

uint32_t NavigationManager::genIndex()
{
	return m_next_index++;
}

void NavigationManager::setup(World *world)
{
	RETURN_IF_NULL(world);
	weak_ptr<GridMap> gridmap = world->getGridMapWeakPtr();
	if (!gridmap.expired()) {
		m_gridmap = gridmap;
	}
}

NavigationManager::PathHandle NavigationManager::queryPath(Vec2f start_point, Vec2f end_point, flt radius, bool consider_entities)
{
	shared_ptr<GridMap> gridmap = m_gridmap.lock();
	if (!gridmap) {
		LOG_WARNING(__FUNCTION__, "path query failed due to invalid gridmap.\n");
		return k_invalid_handle;
	}
	
}

void NavigationManager::releaseHandle(PathHandle handle)
{
	auto it = m_paths.find(handle.m_index);
	if (it != m_paths.end()) m_paths.erase(it);
}

NavigationManager::Path *NavigationManager::getPath(PathHandle handle)
{
	auto it = m_paths.find(handle.m_index);
	if (it == m_paths.end()) return nullptr;
	return &it->second;
}

Pathfinder::~Pathfinder()
{
	// do nothing
}

const NavigationManager::PathHandle NavigationManager::k_invalid_handle;