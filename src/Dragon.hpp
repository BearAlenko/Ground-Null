#pragma once
#include "enemy.hpp"

class Dragon : public Enemy {

	// Creates all the associated render resources and default transform
public:
	static Texture idle_Textures[10];
	static Texture attack_Textures[33];
	static Texture fly_Textures[10];
	static Texture sleep_Texture;
	static Mix_Chunk* m_dragon_fire;
	bool newInit(Player* player, std::vector<Fire>* m_fire, HealthPoints* heathp);
	// Renders the player
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection, mat3& view_2d)override;
	void set_sleep(float sleeptime);
	int largestHP = 50;
	void attack_player(bool att);

	/////////////////////////
    /// stuff for b2 world///
	//b2Body* rigidbody = NULL;
	b2BodyDef dragonBodyDef;
	b2PolygonShape dragonShape;
	b2FixtureDef dragonFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	/////////////////////////


private:
	Texture texture;
	int tex = 0;
	int attack_cd = 0;
};

class Fire_e : public Renderable {
public:
	static Texture attack_e_textures[9];
	static int cd_count;
	bool init(vec2 position, vec2 direction);
	void update(float ms);
	void draw(const mat3& projection, mat3& view_2d)override;
	void destroy();
	bool collides_with(Player& player);
	vec2 get_position()const;
	void set_position(vec2 position);
	vec2 get_bounding_box()const;
	std::vector<vec2> vertex;
	std::vector<uint16_t> indicess;
	bool shouldErase = 0;
	bool hit = 0;
	bool created = 0;
	vec2 enemyposition;

	/////////////////////////
    /// stuff for b2 world///
	b2Body* rigidbody = NULL;
	b2BodyDef dragonFireBodyDef;
	b2PolygonShape dragonFireShape;
	b2FixtureDef dragonFireFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void setDef();
	/////////////////////////

private:

	int damage;
	Texture skill_texture;
	float frametime_prev;
	float start_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	int tex_num;
};

class Thunder_e : public Renderable {
public:
	static Mix_Chunk* m_lighting;
	static Texture thunder_e_textures[5];
	static int cd_count;
	bool init(vec2 position, vec2 direction);
	void update(float ms);
	void draw(const mat3& projection, mat3& view_2d)override;
	void destroy();
	bool collides_with(Player& player);
	vec2 get_position()const;
	void set_position(vec2 position);
	vec2 get_bounding_box()const;
	std::vector<vec2> vertex;
	std::vector<uint16_t> indicess;
	bool shouldErase = 0;
	bool hit = 0;
	bool created = 0;


	/////////////////////////
    /// stuff for b2 world///
	b2Body* rigidbody = NULL;
	b2BodyDef dragonThunderBodyDef;
	b2PolygonShape dragonThunderShape;
	b2FixtureDef dragonThunderFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void setDef();
	/////////////////////////

private:

	int damage;
	Texture skill_texture;
	float frametime_prev;
	float start_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	int tex_num;
};