#pragma once

// internal
#include "GameObject.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "Knight.hpp"
#include "Viking.hpp"
#include "Skull.hpp"
#include "Wolf.hpp"
#include "NPC.hpp"
#include "FrontGround.hpp"
#include "attack_p.hpp"
#include "shield_p.hpp"
#include "Fire.hpp"
#include "Fireball.hpp"
#include "flare_p.hpp"
#include "Smoke.hpp"
#include "level_up.hpp"
#include "DecisionTree.hpp"
#include "Helper.hpp"
#include "Cross.hpp"
#include "Collision.hpp"
#include "HealthPoints.hpp"
#include "Ground.hpp"
#include "JumpOrb.hpp"
#include "Start.hpp"
#include "GoldenOrb.hpp"
#include "KeyUI.hpp"
#include "stone.hpp"
#include "mace.hpp"
#include "SaveLoad.hpp"
#include "BackGround.hpp"
#include "Dragon.hpp"
#include "venom.hpp"
#include "Swamp.hpp"
#include "Pillar.hpp"
#include "Chest.hpp"
#include "ParticleEffect.hpp"
#include "MyContactListener.hpp"
#include "FlareIcon.hpp"
#include "ShieldIcon.hpp"
#include "Quest.hpp"
#include "SleepPotion.hpp"
#include "FullBlood.hpp"
#include "SoulChain.hpp"
#include "resource_manager.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include "Box2D/Box2D.h"


// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class World
{
public:
	World();
	~World();

	// Creates a window, sets up events and begins the game
	bool init(vec2 screen, resource_manager* r);

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms, int fps);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over()const;

	//bounce back when touching the left side of window
	void bounce(float top ,float bot, float left, float right);

	void update_allphysics(float ms);


	void playerVsenemy(Player* p1,Enemy* e1);
	Collision coll;
	std::vector<AABB*> aabbs;

private:
	
	bool spawn_fires();
	// Generates a new enemy
	bool spawn_enemy();
	// Generate cross
	bool spawn_cross();
	// Generate grounds
	bool spawn_grounds();
	// generate orbs
	bool spawn_jumporbs();

	bool spawn_goldenorbs();
	// Generate stone
	bool spawn_stones();

	//generate mace
	bool spawn_mace();

	//generate venom
	bool spawn_venom();

	//generate swamp
	bool spawn_swamp();

	//generate pillar
	bool spawn_pillar();

	//generate chest
	bool spawn_chest();

	void collision_check();
	bool WorldSave();
	bool WorldLoad();


	void scene_change();

	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, int button, int action, int mods);
	void clear();
	void spawn();
	// Window handle
	GLFWwindow* m_window;
	
	// Screen texture
	// The draw loop first renders to this texture, then it is used for the frontGround shader
	GLuint m_frame_buffer;
	Texture screen_texs[3];  //加新场景记得改这里数字！！！！
	Texture background_tex[3];
	int level_size = 3;      //加新场景记得改这里数字！！！！

	// FrontGround effect
	FrontGround m_frontGround;
	BackGround m_backGround;

	DecisionTree decision_tree;

	// Number of npc eaten by the player, displayed in the window title
	unsigned int m_points;

	// Game entities
	Player m_player;
	Helper m_helper;
	Start m_start;
	Arrow m_arrow;
	KeyUI m_keyui;
	NPC m_npc;

	bool orb_jumping;
	bool can_orb_jumping;
	int game_level = 0;
	int previous_level = 0;

	std::vector<Level_Up> lv_ps;
	std::vector<Shield_P*> m_shield_ps;
	std::vector<Flare_P*> m_flare_ps;
	std::vector<Attack_P*> m_attack_ps;
	std::vector<Enemy*> m_enemies;
	std::vector<Fire> m_fire;
	std::vector<Mace*> m_mace;
	std::vector<Venom*> m_venom;
	std::vector<Swamp*> m_swamp;
	std::vector<Pillar*> m_pillar;
	std::vector<Chest*> m_chest;
	std::vector<Cross*> m_cross;
	std::vector<HealthPoints*> m_hps;
	std::vector<Ground*> grounds;
	std::vector<JumpOrb*> jumporbs;
	std::vector<GoldenOrb*> goldenorbs;
	std::vector<Stone*> stones;
	std::vector<Fire_e*> dragon_fires;
	std::vector<Thunder_e*> dragon_thunder;
	

	SoulChain soul_chain;

	Smoke smoke;
	Fireball fireball;

	FlareIcon flareIcon; 
	ShieldIcon shieldIcon;
	SleepPotionIcon potionIcon;
	SoulChainIcon soulIcon;
	QuestManager quest_manager;
	FullBlood fullblood;

	float m_current_speed;
	float m_next_enemy_spawn;
	float m_next_npc_spawn;


	bool helper_show;
	bool start_show;

	//////////////////////////////////////////////////////////////////////////
	//b2world stuff
	b2World* b2world; //pointer to created b2world
	float32 timeStep = 1.0f / 60.0f; //timestep used to step the world
	int32 velocityIterations = 6; // strength of velocity accuracy : higher == more accuracy, more expensive
	int32 positionIterations = 4; // strenth of position accuracy  : higher == more accuracy, more expensive
	b2Vec2 gravity = b2Vec2(0.0f, -10.0f); // gravity in b2 world	
	//b2Vec2 gravity = b2Vec2(0.0f, 0.0f); // test gravity in b2 world	
	b2Body* ballBody;
	MyContactListener myContactListenerInstance;
	b2Body* testBall = NULL;
	//////////////////////////////////////////////////////////////////////////

	vec2 wcs;
	vec2 vcs;
	int countDown = 0;
	int fps = 0;
	
	Mix_Music* m_background_music;
	Mix_Chunk* m_player_dead_sound;
	Mix_Chunk* m_player_eat_sound;
	Mix_Chunk* m_player_eat_golden_orb;
	Mix_Chunk* m_player_eat_cross;
	Mix_Chunk* m_player_shield_break;
	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
	int checked = 0;
};
