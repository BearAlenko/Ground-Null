#pragma once

#include <stdio.h>
#include <GameObject.hpp>

typedef struct
{

	vec2 m_position; // Window coordinates
	vec2 last_position; // last position
	//HealthPoints* healthp;
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
	float shield_cd;
	float flare_cd;
	int jump_countDown;
	int key_no;

	vec2 foot_point;

	float run_speed;
	float jump_speed;

	bool shielded;
	int lifePoint;
	bool m_is_alive; // True if the player is alive
	bool onCube;
	bool needcheck;
	bool can_use_item;

} sPlayer;

typedef struct
{
	vec2 m_position; // Window coordinates
	vec2 last_position; // last position
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	int hp;
	vec2 wcs;
	//Texture enemy_texture;
	float start_time;  // the frame countDown for animation
	int tex_num;
	int run_tex;
	int action_count;
	float step;
	float speed_add;
	float jump_speed;
	float max_step;
	bool flared;
	bool dragon;
	bool fly;
	float ground_height;
	int flareCountDown;

	//std::vector<vec2> forces_to_delete;
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
	int jump;
	int exp;
	int count;
	int id;
	int type;
	//vec3 rageColor = { 1.f,1.f,1.f };
} sEnemy;

typedef struct
{
	bool collided;
	bool erase_flag;
	float start_time;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	int tex_num;
	//b2BodyDef goldenBodyDef;
	//b2PolygonShape goldenShape;
	//b2FixtureDef goldenFixtureDef;

} sGoldenOrb;

typedef struct
{
	bool used;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	size_t m_num_indices; // passe

} sCross;

typedef struct
{
	//float Life_time;
	//float consumed_time;
	float cd;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_direction;
	int tex_num;
	bool inited;
	float rotation;

} sItem;

typedef struct
{
	bool completed;
	int texnum;

} sQuest;

typedef struct
{
	bool show;
	float start_time;
	int tex_num;

} sNewQuest;

typedef struct
{
	int current;
	bool inserted1;

} sQuestManager;

typedef struct
{
	int lockLife;
	vec2 m_position;
} sChest;

typedef struct
{
	int lockLife;
	vec2 m_position;

} sPillar;