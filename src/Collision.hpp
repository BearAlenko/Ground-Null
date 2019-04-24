#pragma once
#include "GameObject.hpp"
#include "vector"
#include <algorithm>
#include "AABB.hpp"
#include <SDL.h>
#include <SDL_mixer.h>



class Collision {
public:

	bool init();

	AABB* Abinit(vec2 p1, vec2 p2, AABBtype type);

	bool testAABBOverlap(AABB* a, AABB* b);

	void SweepAndPrune(std::vector<AABB*> ab, int* countdown);

	int  findAB(value* min, std::vector<AABB*> activelist);

	bool SegVsSeg(vec2 p1s, vec2 p1e, vec2 p2s, vec2 p2e);

	bool LineVsLine(vec2 p1s, vec2 p1e, vec2 p2s, vec2 p2e);

	bool PointOnLine(vec2 p1, vec2 p2s, vec2 p2e);

	bool intersection(std::vector<vec2> poly1, std::vector<vec2> poly2);

	bool insideTest(std::vector<vec2> poly1, std::vector<vec2> poly2, bool convex1, bool convex2, std::vector<uint16_t> indices1, std::vector<uint16_t> indices2);

	void CollisionCatch(AABB* e1, AABB* e2, int* countdown);

	void addvertex(AABB* a1, std::vector<vec2> vertex);

	void addverindices(AABB* a1, std::vector<uint16_t> indices);

	void convex(AABB* a1, bool convex);

	vec2 RemoveOut(AABB* player, AABB* enemy);


private:


};

