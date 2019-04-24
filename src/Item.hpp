#pragma once
#include "GameObject.hpp"
#include "Dragon.hpp"

class Item : public Renderable
{
public:
	virtual bool init(vec2 position, vec2 direction, float time, float cooldown);
	virtual void update(float ms);
	void draw(const mat3& projection, mat3& view_2d)override;
	virtual void addDef() = 0;
	void destroy();
	vec2 get_position()const;
	void set_position(vec2 position);
	virtual void function(Dragon* dragon) = 0;
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);

	b2Body* rigidbody = NULL;

	sItem* getInfo();
	void setInfo(sItem* i1);

	b2BodyDef sleepBodyDef;
	b2PolygonShape sleepShape;
	b2FixtureDef sleepFixtureDef;

	Texture texture;
	float Life_time;
	float consumed_time;
	float cd = 0.f;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	int tex_num;
	bool inited = false;
	float rotation = 0.f;
};
