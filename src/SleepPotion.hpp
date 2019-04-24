#pragma once
#include "Item.hpp"

class SleepPotion : public Item
{
public:
	static Texture sleep_tex;
	bool init(vec2 position, vec2 direction, float sleeptime, float cooldown);
	void function(Dragon* dragon);
	void update(float ms);
	int getEntityType();
	void addDef();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	/////////////////////////
    /// stuff for b2 world///
    //b2Body* rigidbody = NULL;
	//bodyUserData* userdata = NULL;
	/////////////////////////

private:
	float sleep_time;
};
