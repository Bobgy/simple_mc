#include "stdafx.h"

#include <algorithm>

#include "a_star_solver.h"

#include "game/gridmap.h"
#include "game/world.h"

AStarSolver::~AStarSolver()
{

}

const static flt sqrt2 = sqrt(2.0f);

shared_ptr<vector<Vec2f>> AStarSolver::queryPath(
	GridMap *gridmap,
	Vec2f start_point,
	Vec2f end_point,
	flt radius,
	bool consider_entities)
{
	if (gridmap == nullptr) {
		LOG_WARNING(__FUNCTION__, "gridmap is nullptr.\n");
		return nullptr;
	}

	Vec2i vs = Vec2i(round(start_point));
	Vec2i ve = Vec2i(round(end_point));
	auto *range = gridmap->getRange();
	if (!range->isInside(vs) || !range->isInside(ve)) {
		LOG_WARNING(__FUNCTION__, "start or end point is not inside grids.\n");
		return nullptr;
	}
	vs -= range->m_min;
	ve -= range->m_min;
	Vec2ui sz = gridmap->getSize();
	m_grid_infos.resize(sz[0], sz[1]);
	while (!m_opened_list.empty()) {
		m_opened_list.pop();
	}
	
	GridInfo *grid_s = m_grid_infos.getGridInfo(vs);
	assert(grid_s);

	grid_s->g = 0.f;
	grid_s->f = 0.f;
	m_opened_list.push(OpenedGrid{0.f, vs});
	
	flt closest_distance = INF;
	Vec2i closest_point;

	while (!m_opened_list.empty()) {
		OpenedGrid v = m_opened_list.top();
		m_opened_list.pop();
		GridInfo *grid_v = m_grid_infos.getGridInfo(v.pos);
		assert(grid_v);
		if (grid_v->closed) continue;
		grid_v->closed = true;
		flt dis = h_euclidean(v.pos, ve);
		if (sgn(dis - closest_distance) < 0) {
			closest_distance = dis;
			closest_point = v.pos;
		}
		if (v.pos == ve) {
			// found
			break;
		}
		for (int dx = -1; dx <= 1; ++dx) {
			for (int dy = -1; dy <= 1; ++dy) {
				if (dx || dy) {
					Vec2i p_new = v.pos + Vec2i{dx, dy};
					GridInfo *grid_new = m_grid_infos.getGridInfo(p_new);
					if (grid_new == nullptr || grid_new->closed || grid_new->clearance == 0) continue;
					flt len = (dx && dy) ? sqrt2 : 1.f;
					if (sgn((grid_v->g + len) - grid_new->g) < 0) {
						grid_new->g = grid_v->g + len;
						grid_new->f = grid_new->g + h_euclidean(p_new, ve);
						grid_new->parent = v.pos;
						m_opened_list.push(OpenedGrid{grid_new->f, p_new});
					}
				}
			}
		}
	}

	if (closest_distance == INF) return nullptr;
	Vec2i p = closest_point;
	GridInfo *grid = m_grid_infos.getGridInfo(p);
	shared_ptr<vector<Vec2f>> path = make_shared<vector<Vec2f>>();
	if (!path) return nullptr;
	if (sgn(sqr(end_point - Vec2f(p)))) {
		path->push_back(end_point);
	}
	do {
		path->push_back(p);
		p = grid->parent;
		grid = m_grid_infos.getGridInfo(p);
	} while (grid);
	if (sgn(sqr(start_point - Vec2f(vs)))) {
		path->push_back(start_point);
	}
	reverse(path->begin(), path->end());
	return path;
}
