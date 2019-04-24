#pragma once
#include "GameObject.hpp"
#include "Random.hpp"
#include <vector>
struct Particle: public Renderable
{
	static Texture normal_particle_tex;
	~Particle();
	bool init(vec2 position, float direction, Texture tex);
	void update(float ms);
	void draw(const mat3& projection, mat3& vew_2d)override;
	void destroy();
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void reset(vec2 position, float direction, Texture texture);

	vec2   m_Position = { 0.f, 0.f }; // Center point of particle
	vec2   m_Velocity = { 0.f, 0.f }; // Current particle velocity
	vec4   m_Color = {0.f, 0.f, 0.f, 0.f};    // Particle color
	float       m_fRotate = 0.f;  // Rotate the particle the center
	float       m_scale = 0.5f;    // Size of the particle
	float       m_consumedTime = 0.f;
	float       m_LifeTime = 0.f;
	vec2	init_velocity = {0.f, 0.f};
	vec2	acceleration = {0.f, 0.f};
	vec2*	destination = NULL;
	float init_direction = 0.f;
	//GLuint time_buffer_loc;
	//float time_buffer[4];
	//TexturedVertex vertices[4];
	//uint16_t indices[6];
	Texture tex = normal_particle_tex;
};

typedef std::vector<Particle> Particles;



