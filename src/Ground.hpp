#pragma once
#include "GameObject.hpp"
#include <vector>
#include "Box2D/Box2D.h"

class Ground : public Renderable
{
public:
	static Texture normal_ground;
	void draw(const mat3& projection, mat3& view_2d)override;
	bool init(vec2 position);
	//void update(float ms);
	void destroy();
	vec2 get_position() const;
	void set_position(vec2 position);
	std::vector<uint16_t> indicess;
	std::vector<vec2> vertex; //the world coordiante of each cross vertex
	bool checked = false;
	bool playeron = false;
	bool visible = true;
	

	/////////////////////////
    /// stuff for b2 world///
	b2Body* rigidbody = NULL;
	b2BodyDef groundBodyDef;
	b2ChainShape groundShape;
	b2FixtureDef groundFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void setDef();
	b2Vec2 Gstartpoint;
	b2Vec2 Gendpoint;
	/////////////////////////

private:
	Texture ground_texture;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;


}; 