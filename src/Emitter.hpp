#pragma once
#include "GameObject.hpp"
#include "Random.hpp"
#include "Particle.hpp"
#include <vector>


class Emitter {
public:
	Emitter();
	void EmitParticle(Particle& particle);

	float MinimumRadius = 0.f;
	float MaximumRadius = 60.f;

	float MinSpeed = 1.f;
	float MaxSpeed = 2.f;

	float MinLifetime = 1000.f;
	float MaxLifetime = 3000.f;

	vec2 Origin;

};