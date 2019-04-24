#pragma once
#include "enemy.hpp"

class Skull : public Enemy {

	// Creates all the associated render resources and default transform
public:
	static Texture run_Textures[10];
	static Texture attack_Textures[10];
	bool newInit(Player* player, std::vector<Fire>* m_fire, HealthPoints* heathp);

	/////////////////////////
    /// stuff for b2 world///
	//b2Body* rigidbody = NULL;
	b2BodyDef skullBodyDef;
	b2PolygonShape skullShape;
	b2FixtureDef skullFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	/////////////////////////

	// Renders the skull
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection, mat3& view_2d)override;
	int largestHP = 3;
	
};

