#pragma once

#include "GameObject.hpp"

// Player food
class NPC : public Renderable
{
	// Shared between all npc, no need to load one for each instance

public:
	static Texture npc_texture[10];
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all the associated resources
	void destroy();
	
	// Update npc due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the npc
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection, mat3& view_2d)override;

	// Returns the current npc position
	vec2 get_position()const;

	// Sets the new npc position
	void set_position(vec2 position);


	int getEntityType();

	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);

private:
	Texture skill_texture;
	float frametime_prev;
	float start_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	int tex_num;
};