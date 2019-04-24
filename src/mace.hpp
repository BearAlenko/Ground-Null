#pragma once
#include "Enemy.hpp"
#include "Player.hpp"
#include "Box2D/Box2D.h"

class Mace : public Renderable {
public:
	static Texture mace_textures[10];
	bool init(vec2 position);
	void destroy();
	void update(float ms);
	void set_position(vec2 position);
	vec2 get_position()const;
	std::vector<vec2> vertex;
	std::vector<uint16_t> indicess;
	void draw(const mat3& projection, mat3& view_2d)override;
	float m_rotation = 0.f; // in radians
	bool collide_with(Player& player);
	vec2 get_head_a_position();
	vec2 get_head_b_position();
	vec2 get_head_c_position();
	vec2 get_head_d_position();

	/////////////////////////
    /// stuff for b2 world///
	b2Body* rigidbody = NULL;
	b2BodyDef maceBodyDef;
	b2CircleShape maceShape;
	b2FixtureDef maceFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void setDef();
	/////////////////////////

private:
	Texture mace_texture;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 head_a_position = {-1000.f,-700.f}; //the position of the upper left head
	vec2 head_b_position = {-800.f,-550.f}; //the position of the lower right head
	vec2 head_a_original_position = { -1000.f,-700.f };
	vec2 head_b_original_position = { -800.f,-550.f };
	vec2 head_c_position = { -1700.f,-1000.f };
	vec2 head_d_position = { -1500.f, -850.f };
	vec2 head_c_original_position = { -1700.f,-1000.f };
	vec2 head_d_original_position = { -1500.f, -850.f };
	vec2 center = { m_position.x, m_position.y + 300 };
	/*vec2 center1 = {-900.f, -400.f + 300.f};
	vec2 center2 = { -1600.f, -700.f + 300.f };*/
};
