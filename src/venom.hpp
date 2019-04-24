#pragma once
#include "Player.hpp"
#include "Box2D/Box2D.h"


class Venom : public Renderable
{
public:
	static Texture venom_textures[venom_texture_number];
	bool init(vec2 position);
	void update(float ms);
	void draw(const mat3& projection, mat3& view_2d)override;
	void destroy();
	vec2 get_position();
	std::vector<vec2> vertex;
	std::vector<uint16_t> indicess;
	bool state_flag = true;
	bool conlide_with(Player& player);
	bool playerContacting = 0;
	//int get_duration();
   
	/////////////////////////
	/// stuff for b2 world///
	b2Body* rigidbody = NULL;
	b2BodyDef venomBodyDef;
	b2PolygonShape venomShape;
	b2FixtureDef venomFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void setDef();
	void checkPlayer();
	/////////////////////////

private:
	Texture venom_texture;
	//int duration_countdown;
	int start_time = 0;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	int tex_num = 0;
	int period = 2150;
	int count = 0;
};
