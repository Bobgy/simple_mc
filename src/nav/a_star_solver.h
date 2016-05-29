#pragma once

#include <queue>

#include "nav/nav.h"
#include "game/gridmap.h"

#define INF 1e10f

class AStarSolver: public Pathfinder
{
// protected typedef
protected:
	struct GridInfo
	{
		flt f, g;
		bool closed;
		int clearance;
		Vec2i parent;
	};
	struct GridInfoContainer
	{
		uint32_t m_salt = 0;
		GridInfo m_grid_info;
	};
	struct OpenedGrid
	{
		flt f;
		Vec2i pos;
		bool operator < (OpenedGrid r) const {
			return f < r.f;
		}
	};
	
// protected members
protected:
	struct GridInfos {
		GridMap *gridmap;
		uint32_t m_salt = 0;
		Vec2ui m_sz;
		vector<GridInfoContainer> m_containers;
		void resize(size_t w, size_t h) { m_sz = {w, h}; m_containers.resize(w * h); }
		void refresh() { ++m_salt; }
		GridInfo *getGridInfo(size_t i, size_t j) {
			if (i >= m_sz[0] || j >= m_sz[1]) return nullptr;
			size_t offset = m_sz[1] * i + j;
			GridInfoContainer *container = &m_containers[offset];
			if (container->m_salt != m_salt) {
				container->m_salt = m_salt;
				GridInfo *gridinfo = &container->m_grid_info;
				Grid *grid = gridmap->getGridDirectly(offset);
				assert(grid);
				gridinfo->f = INF;
				gridinfo->g = INF;
				gridinfo->closed = false;
				gridinfo->clearance = grid->m_clearance;
				gridinfo->parent = {-1, -1};
			}
		}
		GridInfo *getGridInfo(Vec2ui p) {
			return getGridInfo(p[0], p[1]);
		}
	} m_grid_infos;

	priority_queue<OpenedGrid> m_opened_list;
	
	flt h_euclidean(Vec2i a, Vec2i b) {
		return sqrt((flt)sqr(a - b));
	}

// public methods
public:
	virtual ~AStarSolver();
	virtual shared_ptr<vector<Vec2f>> queryPath(
		GridMap *gridmap,
		Vec2f start_point,
		Vec2f end_point,
		flt radius,
		bool consider_entities = false);
};