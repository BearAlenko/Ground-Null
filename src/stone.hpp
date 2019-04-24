#pragma once
#include "GameObject.hpp"
#include "player.hpp"
#include <vector>
#include "Box2D/Box2D.h"


class Stone : public Renderable
{
public:
	static Mix_Chunk* m_falling_stone;

	static Texture normal_stone;
	void draw(const mat3& projection, mat3& view_2d)override;
	bool init(vec2 position);
	//void update(float ms);
	void destroy();
	vec2 get_position()const;
	void set_position(vec2 position);
	std::vector<uint16_t> indicess;
	std::vector<vec2> vertex; //the world coordiante of each cross vertex
	bool checked = false;
	void update(float ms);
	bool idle;
	bool falling = 0;
	bool up = 0;
	float pre;
	bool scollides_with(Player& player);
	bool collides_with(Player& player);
	bool stone_contacting = 0;
	int near_distance = 100;

	/////////////////////////
    /// stuff for b2 world///
	b2Body* rigidbody = NULL;
	//b2Body* stoneRigidbody = NULL;
	b2BodyDef stoneBodyDef;
	b2CircleShape stoneShape;
	b2PolygonShape stoneSensorShape;
	b2FixtureDef stoneFixtureDef;
	b2FixtureDef stoneSensorFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void setDef();
	void checkPlayer();
	
	/////////////////////////

private:
	Texture stone_texture;
	int falling_sound_countdown = 0;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	int countdown = 0;


};