#pragma once
#include "Player.hpp"
#include "GoldenOrb.hpp"
#include "Box2D/Box2D.h"

class Chest : public Renderable {
public:
	static Texture chest_textures[10];
	bool init(vec2 position);
	void destroy();
	vec2 get_position();
	std::vector<vec2> vertex;
	std::vector<uint16_t> indicess;
	void draw(const mat3& projection, mat3& view_2d)override;
	void update(float ms);
	int tex_num;
	int lockLife = 3;
	bool erase_flag = false;
	bool collidedPlayer = false;
	void collide_with_Player(Player& player);

	sChest* getInfo();
	void setInfo(sChest* c);
	void CheckPlayer();

	//bool collide_with(Attack_P& attack_p);
	///////////////////////////
	//box 2d stuff/////////////
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	b2Body* rigidbody = NULL;
	b2BodyDef chestBodyDef;
	b2PolygonShape chestShape;
	b2FixtureDef chestFixtureDef;
	void setDef();
	GoldenOrb* broken();
	////////////////////////////////
	

private:
	Texture chest_texture;
	vec2 m_position;
	vec2 m_scale;
	
};
