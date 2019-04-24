#pragma once
#include "Player.hpp"
#include "Box2D/Box2D.h"

class Pillar : public Renderable {
public:
	static Texture pillar_textures[3];
	bool init(vec2 position);
	void destroy();
	vec2 get_position();
	bool erase_flag = false;
	std::vector<vec2> vertex;
	std::vector<uint16_t> indicess;
	void draw(const mat3& projection, mat3& view_2d)override;
	void update(float ms);
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	int lockLife = 4;
	int tex_num;
	//void collidewith_attack(Attack_P& attack);
	void setDef();
	bool collide_player = 0;
	void collide_with(Player& player);
	void check_Player();
	sPillar* getInfo();
	void setInfo(sPillar* c);
	
	///////////////////
	//box2d stuff////////

	b2Body* rigidbody = NULL;
	b2BodyDef pillarBodyDef;
	b2PolygonShape pillarShape;
	b2FixtureDef pillarFixtureDef;
	//bodyUserData* userdata = NULL;
	///////////////

private:
	Texture pillar_texture;
	vec2 m_position;
	vec2 m_scale;
};
