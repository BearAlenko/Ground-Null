#pragma once
#include "Enemy.hpp"
#include "Player.hpp"
#include "Box2D/Box2D.h"

class Fireball : public Renderable
{
public:
	static Texture fireball_textures[Ground_fireball_textures];
	bool init(vec2 position, vec2 direction);
	void update(float ms);
	void draw(const mat3& projection, mat3& view_2d)override;
	void destroy();
	bool collides_with(Enemy& enemy);
	vec2 get_position()const;
	void set_position(vec2 position);
	int get_duration();
	vec2 get_bounding_box()const;
	void updatePos();
	void show_fireball(bool status);
	void reset();
	bool fireball_status();
	bool collides_with(Player& player);
	std::vector<vec2> vertex;
	std::vector<uint16_t> indicess;
	void setDef();

	/////////////////////////
    /// stuff for b2 world///
	b2Body* rigidbody = NULL;
	b2BodyDef fireBallBodyDef;
	b2CircleShape fireBallShape;
	b2FixtureDef fireBallFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	/////////////////////////

private:
	Texture texture;
	float frametime_prev;
	int duration_countdown;
	float start_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	int tex_num;

	//physic stuff
	int damping = 20, 
		upper_limit = 1400, 
		bottom = 1650;
	double counter = 0.0;
	bool goup = true;
	bool godown = false;
	bool isOn = false;
};
