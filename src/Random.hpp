#pragma once
#include "GameObject.hpp"
inline float Random()
{
	return rand() / (float)RAND_MAX;
}

inline float RandRange(float fMin, float fMax)
{
	if (fMin > fMax) std::swap(fMin, fMax);
	return (Random() * (fMax - fMin)) + fMin;
}

inline vec2 RandUnitVec()
{
	float x = (Random() * 2.0f) - 1.0f;
	float y = (Random() * 2.0f) - 1.0f;
	vec2 r = normalize(vec2{x, y});

	return r;
}