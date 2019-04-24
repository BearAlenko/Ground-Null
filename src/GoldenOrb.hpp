#pragma once
#include "GameObject.hpp"
#include <vector>
#include "Player.hpp"
#include "Box2D/Box2D.h"


class GoldenOrb : public Renderable
{
public:
	static Texture orb_textures[10];
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
	bool erase_flag = 0;
	sGoldenOrb* getInfo();
	void setInfo(sGoldenOrb* e1);
	bool draw_state = true;

	/////////////////////////
    /// stuff for b2 world///
	b2Body* rigidbody = NULL;
	b2BodyDef goldenBodyDef;
	b2PolygonShape goldenShape;
	b2FixtureDef goldenFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	/////////////////////////

private:
	Texture orb_texture;
	float start_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	int tex_num;
};
