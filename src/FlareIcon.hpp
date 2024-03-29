#pragma once
#include "Player.hpp"

class FlareIcon : public Renderable
{
public:
	static Texture FlareIcon_textures[icon_size];
	bool init(Player* player);
	void update(float ms);
	void draw(const mat3& projection, mat3& view_2d)override;
	void destroy();
	vec2 get_position()const;
	void set_position(vec2 position);
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
private:
	Texture ui_texture;
	Player* player;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
};
