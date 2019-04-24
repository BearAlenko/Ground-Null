#pragma once
#include "GameObject.hpp"
#include "Random.hpp"
#include "Particle.hpp"
#include "Emitter.hpp"
#include <vector>

class ParticleEffect{
public:
	bool init(vec2 position, float direction, const char* fileName, int max, vec2* dest);
	void update(float ms);
	void draw(const mat3& projection, mat3& view_2d);
	void destroy();
	void EmitParticles();
	Particles m_Particles;
	Emitter emitter;
	int max_particles;
	vec2 init_position;
	vec2 m_Force;
	float init_dir;
	Texture tex;
	vec2* destination = NULL;
	//GLuint time_buffer_loc;
	//GLuint position_buffer_loc;
};