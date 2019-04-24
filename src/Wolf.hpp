#pragma once
#include "enemy.hpp"

class Wolf : public Enemy {

	// Creates all the associated render resources and default transform
public:
	static Texture run_Textures[16];
	static Texture attack_Textures[10];
	static Texture jump_Textures[30];
	bool newInit(Player* player, std::vector<Fire>* m_fire, HealthPoints* heathp);
	void check_canAttack();
	// Renders the player
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection, mat3& view_2d)override;
	void update(float ms);
	int largestHP = 50;

	/////////////////////////
    /// stuff for b2 world///
	//b2Body* rigidbody = NULL;
	b2BodyDef wolfBodyDef;
	b2PolygonShape wolfShape;
	b2FixtureDef wolfFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	/////////////////////////
	

private:
	int tex = 0;
	bool idle;
	bool howl;
	bool stand;
	bool bite;
	bool run;
	bool skill;
	bool die;

	
};