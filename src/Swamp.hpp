#pragma once
#include "Player.hpp"
#include "Box2D/Box2D.h"

class Swamp : public Renderable {
public:
	static Texture swamp_textures[10];
	bool init(vec2 position);
	void destroy();
	vec2 get_position();
	std::vector<vec2> vertex;
	std::vector<uint16_t> indicess;
	void draw(const mat3& projection, mat3& view_2d)override;
	void update(float ms);
	void collide_with(Player& player);

	/////////////////////////
    /// stuff for b2 world///
	b2Body* rigidbody = NULL;
	b2BodyDef swampBodyDef;
	b2ChainShape swampShape;
	b2FixtureDef swampFixtureDef;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void setDef();
	///////////////////////

private:
	Texture swamp_texture;
	vec2 m_position; 
	vec2 m_scale; 
};