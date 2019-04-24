#pragma once
#include "Enemy.hpp"
#include "Player.hpp"
#include "Box2D/Box2D.h"


#define orb_jumping_speed_y 30.f;
#define orb_jumping_speed_x 10.f;

class JumpOrb : public Renderable
{
public:
	static Texture orb_textures[5];
	bool init(vec2 position);
	void update(float ms);
	void draw(const mat3& projection, mat3& view_2d)override;
	void destroy();
	bool collides_with(Player& player);
	vec2 get_position()const;
	void set_position(vec2 position);
	vec2 get_bounding_box()const;
	std::vector<vec2> vertex;
	std::vector<uint16_t> indicess;
	bool collided = 0;

	/////////////////////////
    /// stuff for b2 world///
	b2Body* rigidbody = NULL;
	b2BodyDef jumpOrbBodyDef;
	b2CircleShape jumpOrbShape;
	b2FixtureDef jumpOrbFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void checkPlayer();
	void setDef();
	/////////////////////////
	
private:
	Texture orb_texture;
	float start_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	int tex_num;
};

class Arrow : public Renderable
{
public:
	bool init(vec2 position);
	void update(float ms);
	void rotate(float angle);
	void draw(const mat3& projection, mat3& view_2d)override;
	void destroy();
	vec2 get_position()const;
	void set_position(vec2 position);
	void set_angle(float angle);
	float get_angle();
	vec2 get_jumping_force();
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);

private:
	Texture arrow_texture;
	float to_angle;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
};
