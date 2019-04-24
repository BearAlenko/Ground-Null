#pragma once

#include "GameObject.hpp"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include "HealthPoints.hpp"
#include "Physics.hpp"
#include "Ground.hpp"
#include "SaveStruct.hpp"
#include "Box2D/Box2D.h"
//#include "GoldenOrb.hpp"
//#include "Cross.hpp"
//#include "Mace.hpp"

class Enemy;
class NPC;
class Attack_P;
class Shield_P;
class Fire;
class Fireball;
class Flare_P;
class Level_Up;
class Mace;
class Venom;
class Dragon;
class Fire_e;
class Item;
class SleepPotion;
//class GoldenOrb;
//class Cross;
//class Collision;
//struct AABB;

class Player : public Renderable
{
public:
	static Texture stand_textures[10];
	static Texture run_textures[10];
	static Texture jump_textures[10];
	static Texture down_textures[10];
	static Texture die_textures[10];
	static Texture cast_textures[10];
	static Mix_Chunk* m_player_lvlup_sound;
	static Mix_Chunk* m_player_attack_sound;
	static Mix_Chunk* m_player_shield_sound;
	static Mix_Chunk* m_player_explosion_sound;
	static Mix_Chunk* m_swamp;
	static Mix_Chunk* m_swamp2;
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	
	// Update player position based on direction
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	void reset(vec2 position, int exp, int lv);
	
	// Renders the player
	void draw(const mat3& projection, mat3& view_2d)override;

	// Collision routines for enemys and npc
	bool collides_with(const Enemy& enemy);
	bool collides_with(const Fire& fire);
	bool collides_with(const Fireball& ball);
	bool collides_with(const Ground& ground);
	bool collides_with(const Mace& mace);
	bool collides_with(const Venom& venom);
	//bool collides_with(Cross& cross);
	//bool collides_with(Stone& stonee);
	//bool collides_with(const Fire_e& df);

	

	// Returns the current player position
	vec2 get_position()const;

	// Moves the player's position by the specified offset
	void move(vec2 off);

	// Set player rotation in radians
	void set_rotation(float radians);

	// True if the player is alive
	bool is_alive()const;

	// Kills the player, changing its alive state and triggering on death events
	void kill();

	// Set player velocity in one character
	void set_Direction(int dir, bool value);

	Attack_P* attack(bool att);

	Shield_P* cast_shield(bool shielded);

	Flare_P* cast_flare(bool flare);

	void set_shield(bool shielded);

	bool deadOrNot();

	void reset_texnum();

	bool get_casting() const;

	sPlayer* getInfo();

	void setInfo(sPlayer* p);

	void setHPbar(HealthPoints *hp);

	void attacked(int h);

	int get_exp() const;
	int get_level() const;
	void add_exp(int e);
	Level_Up level_u();
	int get_hp() const;
	vec2 get_bounding_box()const;
	int get_keys() const;
	void set_keys(int no);
	void add_keys();
	void delete_keys();
	void set_height_limit(float h);
	void use_item(int index);
	bool check_near_npc();
	bool can_use_item = false;

	bool swamp_sound_switch = false;

	int swamp_sound_countdown = 0;
	
	bool is_item0_usable();
	Physics physic;
	//b2Body body;

	vec2 m_position; // Window coordinates

	vec2 last_position; // last position

	//AABB* AB;

	HealthPoints* healthp;
	std::vector<vec2> vertex;
	std::vector<uint16_t> indicess;

	/////////////////////////
	b2World* b2world;
	/// stuff for b2 world///
	b2Body* rigidbody = NULL;
	b2BodyDef playerBodyDef;
	b2PolygonShape playerShape;
	b2FixtureDef playerFixtureDef;
	//bodyUserData* userdata = NULL;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void checkEnemy();
	void checkFire();
	void checkVenom();
	void checkSwamp();
	void collide_Ground(Ground& ground);
	void checkGround();
	/////////////////////////

	bool shielded;
	int lifePoint = 10;
	int max_hp = 10;
	bool m_is_alive; // True if the player is alive
	bool onCube = 0;
	bool needcheck = 1;
	std::vector<Item*> items;
	int OriginalHeight = 1600.f;

	int countdown = 70;
	int enemyContacting = 0;
	bool fireContacting = 0;
	bool venomContacting = 0;
	bool swampContacting = 0;
	int groundContacting = 0;

	int step_max = player_run_stepMax;

	float shield_cd;
	float flare_cd;
	//bool collidedChest = 0;
	/*int get_texture_height();
	vec2 get_m_scale();*/

private:
	//Collision* coll;
	Texture player_texture;
	
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	float start_time;  // the frame countDown for animation
	int level;
	int total_exp;
	int tex_num;
	bool run;
	bool up;
	//int shield_countDown;
	bool left;
	bool right;
	bool cast;
	float attack_cd;
	int jump_countDown;
	int key_no;
	float ground_height;
	
	

	vec2 foot_point;

	float run_speed;
	float jump_speed;
};
