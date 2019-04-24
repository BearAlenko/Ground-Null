#pragma once
#include "Enemy.hpp"
#include "Player.hpp"
#include "Box2D/Box2D.h"

class Cross : public Renderable
{
public:
	void draw(const mat3& projection, mat3& view_2d)override;
	bool init();
	//void update(float ms);
	void destroy();
	vec2 get_position()const;
	void set_position(vec2 position);
	void set_vertex_position();
	void set_vertex_position2();
	std::vector<vec2> get_vertex_position();
	std::vector<vec2> get_vertex_position2();
	std::vector<uint16_t> indicess;
	std::vector<vec2> vertices; //the world coordiante of each cross vertex
	std::vector<vec2> vertices2;
	bool used = false;
	sCross* getInfo();
	void setInfo(sCross* e1);
	void collidedP(Player& player);



	/////////////////////////
    /// stuff for b2 world///
	b2Body* rigidbody = NULL;
	b2BodyDef crossBodyDef;
	b2PolygonShape crossShape;
	b2PolygonShape crossShape1;
	b2PolygonShape crossShape2;
	b2PolygonShape crossShape3;
	//b2PolygonShape crossShape4;
	b2FixtureDef crossFixtureDef;
	b2FixtureDef crossFixtureDef1;
	b2FixtureDef crossFixtureDef2;
	b2FixtureDef crossFixtureDef3;
	//b2FixtureDef crossFixtureDef4;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void setDef();
	/////////////////////////

private:

	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	size_t m_num_indices; // passed to glDrawElements

	
};