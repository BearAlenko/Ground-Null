#pragma once
#include "GameObject.hpp"
class HealthPoints : public Renderable
{
public:
	static Texture hp_textures[blood_textures];
	bool init(vec2 position, vec2 direction, int hp, int largestHP);
	void update(float ms);
	void draw(const mat3& projection, mat3& view_2d)override;
	void destroy();
	vec2 get_position()const;
	void set_position(vec2 position);
	int current_health_level = 0;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
private:
	int total_health_level = 0;
	Texture hp_texture;
	vec2 m_position; 
	vec2 m_scale; 
	vec2 m_direction;
	int tex_num;
};
