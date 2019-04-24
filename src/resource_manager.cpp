#include "resource_manager.hpp"
#include <vector>
#include <string>
#include <algorithm>


resource_manager::resource_manager()
{
}


resource_manager::~resource_manager()
{
	for (auto& str : texNames) delete str;
}

bool resource_manager::load_textures()
{

	Texture texture2;
	Particle::normal_particle_tex = texture2;
	Particle::normal_particle_tex.load_from_file(normal_particle);
	//////////////////////////////Player///////////////////////////
	int max_texture_num = 33;
	for (int i = 0; i < max_texture_num; i++) {
		char *myArray = new char[8];
		sprintf(myArray, "%d.png", i);
		texNames.push_back(myArray);
	}


	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, character_run, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Player::run_textures[i] = new_texture;
		Player::run_textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, character_stand, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Player::stand_textures[i] = new_texture;
		Player::stand_textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, character_jump, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Player::jump_textures[i] = new_texture;
		Player::jump_textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, character_down, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Player::down_textures[i] = new_texture;
		Player::down_textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, character_die, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Player::die_textures[i] = new_texture;
		Player::die_textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, character_cast, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Player::cast_textures[i] = new_texture;
		Player::cast_textures[i].load_from_file(buffer);
	}


	/////////////Dragon////////////////////////////////////
	for (int i = 0; i < 9; i++) {
		char buffer[256];
		strncpy(buffer, fire_e, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Fire_e::attack_e_textures[i] = new_texture;
		Fire_e::attack_e_textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, dragon_idle, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Dragon::idle_Textures[i] = new_texture;
		Dragon::idle_Textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, dragon_fly, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Dragon::fly_Textures[i] = new_texture;
		Dragon::fly_Textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 33; i++) {
		char buffer[256];
		strncpy(buffer, dragon_attack, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Dragon::attack_Textures[i] = new_texture;
		Dragon::attack_Textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 1; i++) {
		Texture new_texture;
		Dragon::sleep_Texture = new_texture;
		Dragon::sleep_Texture.load_from_file(dragon_sleep);
	}

	/////////////////Golden orb////////////////////////
	for (int i = 0; i <10; i++) {
		char buffer[256];
		strncpy(buffer, ground_goldenorb, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		GoldenOrb::orb_textures[i] = new_texture;
		GoldenOrb::orb_textures[i].load_from_file(buffer);
	}

	/////////////////Mace////////////////////////
	char buffer[256];
	strncpy(buffer, ground_mace, sizeof(buffer));
	strncat(buffer, texNames[0], sizeof(buffer));
	Texture new_texture;
	Mace::mace_textures[0] = new_texture;
	Mace::mace_textures[0].load_from_file(buffer);

	
	/////////////////Venom////////////////////////
	for(int i = 0; i < venom_texture_number; i++){
		char buffer[256];
		strncpy(buffer, ground_venom, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Venom::venom_textures[i] = new_texture;
		Venom::venom_textures[i].load_from_file(buffer);
	}

	/////////////////Swamp////////////////////////
	for(int i = 0; i < 1; i++){ //may change the number here later
		char buffer[256];
		strncpy(buffer, ground_swamp, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Swamp::swamp_textures[i] = new_texture;
		Swamp::swamp_textures[i].load_from_file(buffer);
	}

	/////////////////Pillar////////////////////////
	for (int i = 0; i < 3; i++) { 
		char buffer[256];
		strncpy(buffer, ground_pillar, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Pillar::pillar_textures[i] = new_texture;
		Pillar::pillar_textures[i].load_from_file(buffer);
	}

	/////////////////Chest////////////////////////
	for (int i = 0; i < 2; i++) {
		char buffer[256];
		strncpy(buffer, ground_chest, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Chest::chest_textures[i] = new_texture;
		Chest::chest_textures[i].load_from_file(buffer);
	}
	
	

	///////////////////KeyUI//////////////////////////////
	for (int i = 0; i <5; i++) {
		char buffer[256];
		strncpy(buffer, keyui, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		KeyUI::KeyUI_textures[i] = new_texture;
		KeyUI::KeyUI_textures[i].load_from_file(buffer);
	}

	///////////////jump orb////////////////////////////////
	for (int i = 0; i < 5; i++) {
		char buffer[256];
		strncpy(buffer, ground_jumporb, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		JumpOrb::orb_textures[i] = new_texture;
		JumpOrb::orb_textures[i].load_from_file(buffer);
	}

	/////////////////Helper///////////////////////
	for (int i = 0; i < 3; i++) {
		char buffer[256];
		strncpy(buffer, helper, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Helper::helper_textures[i] = new_texture;
		Helper::helper_textures[i].load_from_file(buffer);
	}

	////////////////Start///////////////////////
	for (int i = 0; i < 2; i++) {
		char buffer[256];
		strncpy(buffer, starter, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Start::start_textures[i] = new_texture;
		Start::start_textures[i].load_from_file(buffer);
	}


	////////////////Ground//////////////////////
	Texture new_texture1;
	Ground::normal_ground = new_texture1;
	Ground::normal_ground.load_from_file(ground1);

	////////////////Stone//////////////////////
	Texture stone_texture1;
	Stone::normal_stone = stone_texture1;
	Stone::normal_stone.load_from_file(stone);

	/////////////////////Enemy/////////////////////////////////
	////////////////////Knight/////////////////////////////////
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, knight_run, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Knight::run_Textures[i] = new_texture;
		Knight::run_Textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, knight_attack, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Knight::attack_Textures[i] = new_texture;
		Knight::attack_Textures[i].load_from_file(buffer);
	}

	//////////////////Viking//////////////////////////////////
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, viking_run, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Viking::run_Textures[i] = new_texture;
		Viking::run_Textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, viking_attack, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Viking::attack_Textures[i] = new_texture;
		Viking::attack_Textures[i].load_from_file(buffer);
	}

	//////////////////Skull//////////////////////////////////
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, skull_run, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Skull::run_Textures[i] = new_texture;
		Skull::run_Textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, skull_attack, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Skull::attack_Textures[i] = new_texture;
		Skull::attack_Textures[i].load_from_file(buffer);
	}

	//////////////////Attack_P/////////////////////////////
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, character_attack, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Attack_P::attack_p_textures[i] = new_texture;
		Attack_P::attack_p_textures[i].load_from_file(buffer);
	}

	//////////////////Flare/////////////////////////////	
	for (int i = 0; i < 27; i++) {
		char buffer[256];
		strncpy(buffer, character_flare, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Flare_P::flare_p_textures[i] = new_texture;
		Flare_P::flare_p_textures[i].load_from_file(buffer);
	}

	//////////////////Level/////////////////////////////	
	for (int i = 0; i < 18; i++) {
		char buffer[256];
		strncpy(buffer, character_levelup, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Level_Up::levelup_textures[i] = new_texture;
		Level_Up::levelup_textures[i].load_from_file(buffer);
	}

	//////////////////shield_P/////////////////////////////
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, character_shield, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Shield_P::shield_p_textures[i] = new_texture;
		Shield_P::shield_p_textures[i].load_from_file(buffer);
	}

	//////////////////Fire/////////////////////////////
	for (int i = 0; i < Ground_fire_textures; i++) {
		char buffer[256];
		strncpy(buffer, ground_fire, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Fire::fire_textures[i] = new_texture;
		Fire::fire_textures[i].load_from_file(buffer);
	}

	//////////////////smoke/////////////////////////////
	for (int i = 0; i < Ground_smoke_textures; i++) {
		char buffer[256];
		strncpy(buffer, ground_smoke, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Smoke::smoke_textures[i] = new_texture;
		Smoke::smoke_textures[i].load_from_file(buffer);
	}

	//////////////////Wolf run/////////////////////////////

	for (int i = 0; i < Wolf_run_textures; i++) {
		char buffer[256];
		strncpy(buffer, wolf_run, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Wolf::run_Textures[i] = new_texture;
		Wolf::run_Textures[i].load_from_file(buffer);
	}

	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, wolf_attack, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Wolf::attack_Textures[i] = new_texture;
		Wolf::attack_Textures[i].load_from_file(buffer);
	}

	std::vector<char*> wolf_jump_texture;
	wolf_jump_texture.push_back("0.png");
	wolf_jump_texture.push_back("1.png");
	wolf_jump_texture.push_back("2.png");
	wolf_jump_texture.push_back("3.png");
	wolf_jump_texture.push_back("4.png");
	wolf_jump_texture.push_back("5.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("6.png");
	wolf_jump_texture.push_back("7.png");
	wolf_jump_texture.push_back("7.png");
	wolf_jump_texture.push_back("8.png");
	wolf_jump_texture.push_back("9.png");

	for (int i = 0; i < 30; i++) {
		char buffer[256];
		strncpy(buffer, wolf_jump, sizeof(buffer));
		strncat(buffer, wolf_jump_texture[i], sizeof(buffer));
		Texture new_texture;
		Wolf::jump_Textures[i] = new_texture;
		Wolf::jump_Textures[i].load_from_file(buffer);
	}

	for (int i = 0; i < Ground_fireball_textures; i++) {
		char buffer[256];
		strncpy(buffer, ground_fireball, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Fireball::fireball_textures[i] = new_texture;
		Fireball::fireball_textures[i].load_from_file(buffer);
	}

	for (int i = 0; i < blood_textures; i++) {
		char buffer[256];
		strncpy(buffer, blood_path, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		HealthPoints::hp_textures[i] = new_texture;
		HealthPoints::hp_textures[i].load_from_file(buffer);
	}

	for (int i = 0; i < 5; i++) {
		char buffer[256];
		strncpy(buffer, thunder_e, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Thunder_e::thunder_e_textures[i] = new_texture;
		Thunder_e::thunder_e_textures[i].load_from_file(buffer);
	}
	Texture new_texture3;
	SleepPotion::sleep_tex = new_texture3;
	SleepPotion::sleep_tex.load_from_file(potion);

	for (int i = 0; i < icon_size; i++) {
		char buffer[256];
		strncpy(buffer, icon_flare, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		FlareIcon::FlareIcon_textures[i] = new_texture;
		FlareIcon::FlareIcon_textures[i].load_from_file(buffer);
	}
	
	for (int i = 0; i < icon_size; i++) {
		char buffer[256];
		strncpy(buffer, icon_shield, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		ShieldIcon::ShieldIcon_textures[i] = new_texture;
		ShieldIcon::ShieldIcon_textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < icon_size; i++) {
		char buffer[256];
		strncpy(buffer, icon_potion, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		SleepPotionIcon::ShieldIcon_textures[i] = new_texture;
		SleepPotionIcon::ShieldIcon_textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < icon_size; i++) {
		char buffer[256];
		strncpy(buffer, icon_soulchain, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		SoulChainIcon::ShieldIcon_textures[i] = new_texture;
		SoulChainIcon::ShieldIcon_textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 8; i++) {
		char buffer[256];
		strncpy(buffer, quest, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		Quest::textures[i] = new_texture;
		Quest::textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 1; i++) {
		char buffer[256];
		strncpy(buffer, questmanager, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		QuestManager::texture = new_texture;
		QuestManager::texture.load_from_file(buffer);
	}

	for (int i = 0; i < FullBlood_Size; i++) {
		char buffer[256];
		strncpy(buffer, full_blood, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		FullBlood::FullBlood_textures[i] = new_texture;
		FullBlood::FullBlood_textures[i].load_from_file(buffer);
	}
	for (int i = 0; i < 10; i++) {
		char buffer[256];
		strncpy(buffer, npc, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		NPC::npc_texture[i] = new_texture;
		NPC::npc_texture[i].load_from_file(buffer);
	}
	for (int i = 0; i < 6; i++) {
		char buffer[256];
		strncpy(buffer, newquest, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		NewQuest::texture[i] = new_texture;
		NewQuest::texture[i].load_from_file(buffer);
	}
	for (int i = 0; i < 1; i++) {
		char buffer[256];
		strncpy(buffer, soulchain, sizeof(buffer));
		strncat(buffer, texNames[i], sizeof(buffer));
		Texture new_texture;
		SoulChain::texture = new_texture;
		SoulChain::texture.load_from_file(buffer);
	}
	return true;
}

bool resource_manager::load_sfx() {

	Player::m_player_lvlup_sound = Mix_LoadWAV(audio_path("player_level_up.wav"));
	Player::m_player_attack_sound = Mix_LoadWAV(audio_path("player_attack.wav"));
	Player::m_player_shield_sound = Mix_LoadWAV(audio_path("player_shield.wav"));
	Player::m_player_explosion_sound = Mix_LoadWAV(audio_path("explosion.wav"));
	Enemy::m_enemy_attack_sound = Mix_LoadWAV(audio_path("monster_attack.wav"));
	Enemy::m_enemy_wolf_bite = Mix_LoadWAV(audio_path("wolf_bite.wav"));
	Dragon::m_dragon_fire = Mix_LoadWAV(audio_path("dragon_fire.wav"));
	Thunder_e::m_lighting = Mix_LoadWAV(audio_path("lighting.wav"));
	Stone::m_falling_stone = Mix_LoadWAV(audio_path("falling_stone.wav"));
	Attack_P::m_chest_hit = Mix_LoadWAV(audio_path("chest_hit.wav"));
	Attack_P::m_chest_break = Mix_LoadWAV(audio_path("chest_break.wav"));
	Attack_P::m_wooden_hit = Mix_LoadWAV(audio_path("wooden_hit.wav"));
	Attack_P::m_wooden_break = Mix_LoadWAV(audio_path("wooden_break.wav"));
	Player::m_swamp = Mix_LoadWAV(audio_path("swamp.wav"));
	Player::m_swamp2 = Mix_LoadWAV(audio_path("swamp2.wav"));
	return true;
}

