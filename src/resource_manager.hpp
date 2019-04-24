#pragma once
#include "player.hpp"
#include "enemy.hpp"
#include "Knight.hpp"
#include "Viking.hpp"
#include "Wolf.hpp"
#include "NPC.hpp"
#include "attack_p.hpp"
#include "FrontGround.hpp"
#include "Fire.hpp"
#include "Smoke.hpp"
#include "shield_p.hpp"
#include "flare_p.hpp"
#include "level_up.hpp"
#include "Skull.hpp"
#include "Helper.hpp"
#include "Fireball.hpp"
#include "HealthPoints.hpp"
#include "JumpOrb.hpp"
#include "Ground.hpp"
#include "Start.hpp"
#include "GoldenOrb.hpp"
#include "KeyUI.hpp"
#include "stone.hpp"
#include "mace.hpp"
#include "Swamp.hpp"
#include "Pillar.hpp"
#include "Chest.hpp"
#include "Dragon.hpp"
#include "venom.hpp"
#include "Particle.hpp"
#include "SleepPotion.hpp"
#include "ShieldIcon.hpp"
#include "FlareIcon.hpp"
#include "Quest.hpp"
#include "FullBlood.hpp"
#include "SoulChain.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
class resource_manager {
public:
	std::vector<char*> texNames;
	resource_manager();
	~resource_manager();
	bool load_textures();
	bool load_sfx();
};
