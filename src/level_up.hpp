#pragma once
#include "Player.hpp"

class Level_Up: public Renderable
{
public:
	static Texture levelup_textures[18];
	bool init(vec2 position, vec2 direction, Player* player);
	void update(float ms);
	void draw(const mat3& projection, mat3& view_2d)override;
	void destroy();
	vec2 get_position()const;
	void set_position(vec2 position);
	int get_duration();
	Player* player = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
private:
	Texture lv_texture;
	int duration_countdown;
	float start_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	int tex_num;
};