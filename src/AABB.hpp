#pragma once

#include "Cross.hpp"
#include "shield_p.hpp"
#include "flare_p.hpp"
#include "attack_p.hpp"
#include "Fireball.hpp"
#include "Ground.hpp"
#include "JumpOrb.hpp"
#include "GoldenOrb.hpp"
#include "stone.hpp"
#include "mace.hpp"
#include "venom.hpp"
#include "Dragon.hpp"

struct AABB;
struct value;

enum AABBtype
{
	player_a, enemy_a, fire_a, cross_a, shield_a, attack_a, flare_a, fireball_a, ground_a , jump_orb_a , gold_orb_a , stone_a , mace_a , venom_a, dragonfire_a
};

struct AABB {
	value* min;
	value* max;
	AABBtype type; // decide if it is a player's AABB or something
	Player* player;
	Enemy* enemy;
	Fire* fire;
	Cross* cross;
	Shield_P* shield;
	Flare_P* flare;
	Attack_P* attack;
	Fireball* fireball;
	Ground* ground;
	JumpOrb* orb;
	GoldenOrb* gold;
	Stone* sstone;
	Mace* mace;
	Venom* venom;
	Fire_e* df;
	std::vector<uint16_t> indices;
	std::vector<vec2> vextex;
	std::vector<Vertex> vextex1;
	bool convex = 1;
	//void destroy();
	bool checked = 0;
	~AABB() {
		delete min;
		delete max;
	}
};


struct value {
	bool min;
	vec2 var;
	AABB *master;
};

typedef struct AABB AABB;
typedef struct value value;

