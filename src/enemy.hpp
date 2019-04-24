#pragma once

#include "GameObject.hpp"
#include "Player.hpp"
#include "Fire.hpp"
#include <vector>
#include "Smoke.hpp"
#include "HealthPoints.hpp"
#include "Physics.hpp"
#include "Box2D/Box2D.h"



// Player enemy 


class Enemy : public Renderable
{
	
public:
	static Mix_Chunk* m_enemy_attack_sound;
	static Mix_Chunk* m_enemy_wolf_bite;

	Player* player;
	std::vector<Fire>* fires;
	HealthPoints* healthpoint;
	bool init(char enemy[], Player* player, std::vector<Fire>* m_fire, HealthPoints* healthp);

	//bool init(char enemy[]);
	// Releases all the associated resources
	void destroy();

	// Update enemy due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Returns the current enemy position
	vec2 get_position()const;

	// Sets the new enemy position
	void set_position(vec2 position);

	// Returns the enemy' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

	virtual void attack_player(bool att);

	void attacked(int damage);

	void setJump();

	void setRun(int scalar);


	int get_hp()const;
	void set_height_limit(float h);

	void check_ground();
	void check_canAttack();
	void check_dead();
	void check_healthy();
	void check_animation_done();
	void check_touch_fire();
	void check_range();
	void setRage();
	void check_left();
	void move(vec2 off);
	sEnemy* getInfo();
	void setInfo(sEnemy* e1);

	vec2 m_position; // Window coordinates
	vec2 last_position; // last position
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	int hp;
	vec2 wcs;
	Texture enemy_texture;

	float start_time;  // the frame countDown for animation
	int tex_num;
	int run_tex = 0;
	int action_count;
	float step;
	float speed_add;
	float jump_speed = 8.0;
	float max_step;
	bool flared = false;
	int flareCountDown = 0;
	int type;
	bool shoulderase = 0;
	bool onCube = 0;
	bool needcheck = 1;
	float sleep_time;
	float distort = 0.f;
	bool chained = false;

	//int facing;   // 0 right, 1 left

	Physics physic;
	std::vector<vec2> forces_to_delete;

	std::vector<uint16_t> indicess;
	std::vector<vec2> vertex;
	float ground_height;
	bool up;
	bool down;
	bool left;
	bool attack;
	bool run;
	bool dead;
	bool animation_done;
	bool touch_fire;
	bool range;
	bool boss;
	bool canAttack;
	bool healthy;
	bool rage;
	float cd;
	bool enemyon = false;
	bool echecked = false;
	bool dragon = false;
	bool fly = false;
	int groundContacting = 0;

	int enemy_sound_countdown = 0;
	int wolf_sound_countdown = 0;
	int dragon_sound_countdown = 0;
	int jump;
	int exp;
	int count = 30;
	int OriginalHeight = 1600.f;
	vec3 rageColor = { 1.f,1.f,1.f };
	b2Body* rigidbody = NULL;
};
