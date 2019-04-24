// Header
#include "world.hpp"
#include "../ext/stb_image/stb_image.h"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <cmath>
#include <time.h>

//sfml test
#include <SFML/Graphics.hpp>
//#include "Box2D/Box2D.h"



// Same as static in c, local to compilation unit
namespace
{
	const size_t MAX_ENEMIES = 2;
	const size_t MAX_NPC = 0;
	const size_t ENEMY_DELAY_MS = 2000;
	const size_t NPC_DELAY_MS = 5000;

	namespace
	{
		void glfw_err_cb(int error, const char* desc)
		{
			fprintf(stderr, "%d: %s", error, desc);
		}
	}
}

World::World() :
	m_points(0),
	m_next_enemy_spawn(0.f),
	m_next_npc_spawn(0.f)
{
	// Seeding rng with random device
	m_rng = std::default_random_engine(std::random_device()());
}

World::~World()
{
	//delete m_background_music;
	//delete m_player_dead_sound;
	//delete m_player_eat_sound;
	//delete m_window;
}

// World initialization
bool World::init(vec2 screen, resource_manager* r)
{
	// BOX 2D TES LINE STARTS

	// Construct a world object, which will hold and simulate the rigid bodies.
	b2world = new b2World(gravity);
	b2world->SetContactListener(&myContactListenerInstance);
	// Define the ground body.

	
	b2BodyDef ballBodyDef;
	ballBodyDef.type = b2_dynamicBody;
	ballBodyDef.position.Set(0, 0);
	ballBody = b2world->CreateBody(&ballBodyDef);

	b2CircleShape circle;
	circle.m_radius = 20;
	b2FixtureDef ballShapeDef;
	ballShapeDef.shape = &circle;
	ballShapeDef.density = 1.0f;
	ballShapeDef.friction = 0.2f;
	ballShapeDef.restitution = 0.8f;
	ballBody->CreateFixture(&ballShapeDef);


	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0);

	b2Body *groundBody = b2world->CreateBody(&groundBodyDef);
	b2EdgeShape groundEdge;
	b2FixtureDef boxShapeDef;
	boxShapeDef.shape = &groundEdge;

	//wall definitions
	groundEdge.Set(b2Vec2(0, 0), b2Vec2(1920, 0));
	groundBody->CreateFixture(&boxShapeDef);
	groundEdge.Set(b2Vec2(0, 0), b2Vec2(0, 100));
	groundBody->CreateFixture(&boxShapeDef);

	groundEdge.Set(b2Vec2(0, 100),
		b2Vec2(1920, 100));
	groundBody->CreateFixture(&boxShapeDef);

	groundEdge.Set(b2Vec2(1920,100),
		b2Vec2(1920, 0));
	groundBody->CreateFixture(&boxShapeDef);



	//BOX 2D TEST LINE ENDSSSSSS

	for (int32 i = 0; i < 60; ++i)
	{
		//b2world->Step(timeStep, velocityIterations, positionIterations);
		
		if (ballBody->GetContactList() || groundBody->GetContactList()) 
		{
			//printf("hit \n");
		}
		else { 
			//printf("falling down \n");
		}

		b2Vec2 position = ballBody->GetPosition();
		float32 angle = ballBody->GetAngle();
		//printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
	}
	
	
	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization
	// Core Opengl 3.
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);
	m_window = glfwCreateWindow((int)screen.x, (int)screen.y, "GroundNull", nullptr, nullptr);
	if (m_window == nullptr)
		return false;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	gl3w_init();

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(m_window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((World*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
	auto mouse_button_callback = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((World*)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1, _2); };
	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetMouseButtonCallback(m_window, mouse_button_callback);

	// Create a frame buffer
	m_frame_buffer = 0;
	glGenFramebuffers(1, &m_frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// Initialize the screen texture
	screen_texs[0].load_from_file(backGround0);
	screen_texs[1].load_from_file(backGround1);
	screen_texs[2].load_from_file(backGround2);
	background_tex[0].load_from_file(r_backGround0);
	background_tex[1].load_from_file(r_backGround1);
	background_tex[2].load_from_file(r_backGround2);
	m_frontGround.frontGround = screen_texs[game_level];
	m_backGround.backGround = background_tex[game_level];
	//-------------------------------------------------------------------------
	// Loading music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	m_background_music = Mix_LoadMUS(audio_path("music.wav"));
	m_player_dead_sound = Mix_LoadWAV(audio_path("player_dead.wav"));
	m_player_eat_sound = Mix_LoadWAV(audio_path("player_eat.wav"));
	m_player_eat_golden_orb = Mix_LoadWAV(audio_path("eat_golden_orb.wav"));
	m_player_eat_cross = Mix_LoadWAV(audio_path("health.wav"));
	m_player_shield_break = Mix_LoadWAV(audio_path("shield_break.wav"));
	if (m_player_shield_break == nullptr || m_background_music == nullptr || m_player_dead_sound == nullptr || m_player_eat_sound == nullptr || m_player_eat_golden_orb == nullptr || m_player_eat_cross == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("music.wav"),
			audio_path("player_dead.wav"),
			audio_path("player_eat.wav"),
			audio_path("eat_golden_orb.wav"),
			audio_path("health.wav"),
			audio_path("explosion.wav"),
			audio_path("shield_break.wav"));
		return false;
	}

	// Playing background music undefinitely
	Mix_PlayMusic(m_background_music, -1);
	fprintf(stderr, "Loaded music\n");

	m_current_speed = 1.f;


	/*
	//initializing box2d world
	b2Vec2 gravityy(0, -9.8); //normal earth gravity, 9.8 m/s/s straight down!
	b2world= new b2World(gravityy);

	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	for (int32 i = 0; i < 60; ++i)
	{
		b2world->Step(timeStep, velocityIterations, positionIterations);
		printf("one advance");

	}
	*/
	r->load_textures();
	r->load_sfx();
	bool status = m_player.init() && m_backGround.init() && m_frontGround.init() && fireball.init({ -4000.f, 1400.f }, { 1,0 });

	/*
	AABB* fireball_ab = coll.Abinit(fireball.vertex[2], fireball.vertex[0], fireball_a);
	coll.addverindices(fireball_ab, fireball.indicess);
	coll.addvertex(fireball_ab, fireball.vertex);
	fireball_ab->fireball = &fireball;
	aabbs.emplace_back(fireball_ab);
	*/

	
	fireball.setDef();
	fireball.rigidbody = b2world->CreateBody(&(fireball.fireBallBodyDef));
	fireball.rigidbody->CreateFixture(&(fireball.fireBallFixtureDef));
	fireball.rigidbody->SetUserData(&fireball);
	


	HealthPoints* hp_player = new HealthPoints;
	hp_player->init(m_player.get_position(), { 1,0 }, m_player.get_hp(), 10);
	m_player.setHPbar(hp_player);
	m_hps.emplace_back(hp_player);


	helper_show = false;
	can_orb_jumping = false;
	orb_jumping = false;
	start_show = true;
	status = status && m_helper.init() &&
		decision_tree.init() && spawn_fires() && spawn_cross() && spawn_mace() && spawn_venom() && spawn_swamp() && spawn_pillar() && spawn_enemy() && spawn_jumporbs() && spawn_goldenorbs()
		&& spawn_grounds() && spawn_chest() && spawn_stones() && m_start.init() && m_arrow.init({ 0.f, 0.f }) && smoke.init(m_player.get_position(), { 1,0 });
	
	soul_chain.init(&m_player, NULL);

	//creating AABB for m_player
	
	AABB* player_ab = coll.Abinit(m_player.vertex[2], m_player.vertex[0], player_a);
	coll.addverindices(player_ab, m_player.indicess);
	coll.addvertex(player_ab, m_player.vertex);
	player_ab->player = &m_player;
	aabbs.emplace_back(player_ab);
	

	m_player.rigidbody = b2world->CreateBody(&m_player.playerBodyDef);
	m_player.rigidbody->CreateFixture(&m_player.playerFixtureDef);
	m_player.rigidbody->SetUserData(&m_player);


	m_keyui.init(&m_player);
	flareIcon.init(&m_player);
	shieldIcon.init(&m_player);
	potionIcon.init(&m_player);
	soulIcon.init(&soul_chain);
	quest_manager.init(&m_player);

	fullblood.init(&m_player);
	m_player.b2world = b2world;

	SleepPotion* sp = new SleepPotion;
	sp->init(vec2{ 0.f, 0.f }, m_player.physic.face_right ? vec2{ 1.f, 0.f } : vec2{ -1.f, 0.f }, 300.f, 5000.f);
	sp->addDef();
	sp->rigidbody = b2world->CreateBody(&sp->sleepBodyDef);
	sp->rigidbody->CreateFixture(&sp->sleepFixtureDef);
	sp->rigidbody->SetUserData(sp);
	m_player.items.emplace_back(sp);
	return status;
}


// Releases all the associated resources
void World::destroy()
{
	glDeleteFramebuffers(1, &m_frame_buffer);

	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);
	if (m_player_dead_sound != nullptr)
		Mix_FreeChunk(m_player_dead_sound);
	if (m_player_eat_sound != nullptr)
		Mix_FreeChunk(m_player_eat_sound);
	if (m_player_eat_golden_orb != nullptr)
		Mix_FreeChunk(m_player_eat_golden_orb);
	if (m_player_eat_cross != nullptr)
		Mix_FreeChunk(m_player_eat_cross);
	if (m_player_shield_break != nullptr)
		Mix_FreeChunk(m_player_shield_break);
	if (Player::m_player_lvlup_sound != nullptr)
		Mix_FreeChunk(Player::m_player_lvlup_sound);
	if (Player::m_player_attack_sound != nullptr)
		Mix_FreeChunk(Player::m_player_attack_sound);
	if (Player::m_player_shield_sound != nullptr)
		Mix_FreeChunk(Player::m_player_shield_sound);
	if (Player::m_player_explosion_sound != nullptr)
		Mix_FreeChunk(Player::m_player_explosion_sound);
	if (Enemy::m_enemy_attack_sound != nullptr)
		Mix_FreeChunk(Enemy::m_enemy_attack_sound);
	if (Enemy::m_enemy_wolf_bite != nullptr)
		Mix_FreeChunk(Enemy::m_enemy_wolf_bite);
	if (Dragon::m_dragon_fire != nullptr)
		Mix_FreeChunk(Dragon::m_dragon_fire);
	if (Thunder_e::m_lighting != nullptr)
		Mix_FreeChunk(Thunder_e::m_lighting);
	if (Stone::m_falling_stone != nullptr)
		Mix_FreeChunk(Stone::m_falling_stone);
	if (Attack_P::m_chest_hit != nullptr)
		Mix_FreeChunk(Attack_P::m_chest_hit);
	if (Attack_P::m_chest_break != nullptr)
		Mix_FreeChunk(Attack_P::m_chest_break);
	if (Attack_P::m_wooden_hit != nullptr)
		Mix_FreeChunk(Attack_P::m_wooden_hit);
	if (Attack_P::m_wooden_break != nullptr)
		Mix_FreeChunk(Attack_P::m_wooden_break);
	if (Player::m_swamp != nullptr)
		Mix_FreeChunk(Player::m_swamp);
	if (Player::m_swamp2 != nullptr)
		Mix_FreeChunk(Player::m_swamp2);

	Mix_CloseAudio();

	m_player.destroy();
	m_keyui.destroy();
	fullblood.destroy();
	m_arrow.destroy();
	m_helper.destroy();
	m_start.destroy();
	m_frontGround.destroy();
	m_backGround.destroy();
	fireball.destroy();
	smoke.destroy();
	flareIcon.destroy();
	shieldIcon.destroy();
	potionIcon.destroy();
	soulIcon.destroy();
	m_npc.destroy();

	for (auto& enemy : m_enemies)
	{
		enemy->destroy();
		delete enemy;
	}
	for (auto& hp : m_hps) {
		hp->destroy();
		delete hp;
	}
	for (auto& attack_p : m_attack_ps) {
		attack_p->destroy();
		delete attack_p;
	}
	for (auto& shield_p : m_shield_ps) {
		shield_p->destroy();
		delete shield_p;
	}
	for (auto& flare_p : m_flare_ps) {
		flare_p->destroy();
		delete flare_p;
	}
	for (auto& lv : lv_ps) {
		lv.destroy();
		//delete &lv;
	}

	for (auto&fire : m_fire) {
		fire.destroy();
		//delete &fire;
	}
	for (auto& g : grounds) {
		g->destroy();
		delete g;
	}
	for (auto& o : jumporbs) {
		o->destroy();
		delete o;
	}
	for (auto& g : goldenorbs) {
		g->destroy();
		delete g;
	}
	for (auto& m : m_mace) {
		m->destroy();
		delete m;
	}
	for (auto& m : m_venom) {
		m->destroy();
		delete m;
	}
	for (auto& m : m_swamp) {
		m->destroy();
		delete m;
	}
	for (auto& m : m_pillar) {
		m->destroy();
		delete m;
	}

	for (auto& m : m_chest) {
		m->destroy();
		delete m;
	}
	for (auto& s : stones) {
		s->destroy();
		delete s;
	}
	
	for (auto& a : aabbs) {
		delete a;
	}
	for (auto& a : m_cross) {
		a->destroy();
		delete a;
	}
	for (auto& a : dragon_fires) {
		a->destroy();
		delete a;
	}
	for (auto& a : dragon_thunder) {
		a->destroy();
		delete a;
	}
	quest_manager.destroy();
	m_enemies.clear();
	m_attack_ps.clear();
	m_shield_ps.clear();
	m_flare_ps.clear();
	lv_ps.clear();
	m_fire.clear();
	m_cross.clear();
	m_mace.clear();
	m_venom.clear();
	m_swamp.clear();
	m_pillar.clear();
	m_chest.clear();
	grounds.clear();
	m_hps.clear();
	decision_tree.destroy();
	goldenorbs.clear();
	stones.clear();
	dragon_fires.clear();
	dragon_thunder.clear();
	aabbs.clear();
	soul_chain.destroy();
	delete b2world;
	glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms, int f)
{
	if (start_show || m_current_speed == 0) return true;
	fps = f;
	m_player.last_position = m_player.get_position();
	for (auto& enemy : m_enemies) {

		enemy->last_position = enemy->get_position();
	}



	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	vec2 screen = { (float)w, (float)h };

	if (countDown != 0)
		countDown--;
	bounce(-screen_texs[game_level].height, screen_texs[game_level].height, -screen_texs[game_level].width, screen_texs[game_level].width);   //set player bounce when reaching the boundary of ground

	int i = 0;
	collision_check();
	int j = 0;


	quest_manager.update(elapsed_ms);
	if (quest_manager.get_empty() && (m_player.check_near_npc() || quest_manager.inserted1)) quest_manager.insert_quest(); // when npc fixed and if player is near npc
	if (game_level == 0) {
		if (m_player.get_keys() == 4 && m_enemies.size() == 0) {
			game_level = 2;
			scene_change();
			quest_manager.current = 3;
		}
	}
	if (game_level == 2) {
		if (m_player.get_keys() == 4) {
			game_level = 1;
			scene_change();
			quest_manager.current = 4;
		}
	}
	//////////////////////////Orb Jumping////////////////////////////////////////////////

	int countorbs = 0;
	for (auto& orb : jumporbs) {
		if (orb->collided) {
			can_orb_jumping = true;
			break;
		}
		countorbs++;
	}
	if (countorbs == jumporbs.size()) can_orb_jumping = false;

	if (orb_jumping) return true;           ////// time stopping when orb_jumping. 

	/////////////////////////////////////////////////////////////////////////////////////

	for (auto it = m_attack_ps.begin(); it != m_attack_ps.end();) {

		Attack_P* attack_p = *it;

			if (attack_p->get_duration() > 0 && attack_p->hit) {
				smoke.set_duration(8);
				smoke.set_position(attack_p->enemyposition);
			}
		if (attack_p->shouldErase || attack_p->get_duration()<=0) {
			
			it = m_attack_ps.erase(it);

			/*
			int index = 0;
			for (int i = 0; i < aabbs.size(); i++) {

				if (aabbs[i]->attack == attack_p) {
					delete aabbs[i];
					index = i;
					break;
				}
			}
			aabbs.erase(aabbs.begin() + index);
			*/
			attack_p->destroy();
			b2world->DestroyBody(attack_p->rigidbody);
			delete attack_p;
			continue;
		}
		it++;		
	}

	if (smoke.get_duration() > 0)
		smoke.update(elapsed_ms*m_current_speed);
	
	//mace
	for (auto&mace : m_mace) {
		mace->update(elapsed_ms);
	}



	// Checking Flare_P - Enemy coliisions

	for (auto it = m_flare_ps.begin(); it != m_flare_ps.end();) {
		Flare_P* flare_p = *it;
		if (flare_p->get_duration() <= 0) {
			flare_p->shouldErase = true;
			it = m_flare_ps.erase(it);

			/*
			int index = 0;
			for (int i = 0; i < aabbs.size(); i++) {
				if (aabbs[i]->flare == flare_p) {
					delete aabbs[i];
					index = i;
					break;
				}
			}
			aabbs.erase(aabbs.begin() + index);
			*/
			flare_p->destroy();
			b2world->DestroyBody(flare_p->rigidbody);
 			delete flare_p;	
			continue;
		}
		else
		{
			it++;
		}
	}

	for (auto it = dragon_fires.begin(); it != dragon_fires.end();) {
		Fire_e* flare_p = *it;
		if (flare_p->shouldErase) {
			it = dragon_fires.erase(it);

			/*
			int index = 0;
			for (int i = 0; i < aabbs.size(); i++) {
				if (aabbs[i]->df == flare_p) {
					delete aabbs[i];
					index = i;
					break;
				}
			}
			aabbs.erase(aabbs.begin() + index);
			*/
			flare_p->destroy();
			b2world->DestroyBody(flare_p->rigidbody);
			delete flare_p;
			continue;
		}
		else
		{
			it++;
		}
	}

	for (auto it = dragon_thunder.begin(); it != dragon_thunder.end();) {
		Thunder_e* flare_p = *it;
		if (flare_p->shouldErase) {
			it = dragon_thunder.erase(it);
			int index = 0;
			b2world->DestroyBody(flare_p->rigidbody);
			flare_p->destroy();
			delete flare_p;
			continue;
		}
		else
		{
			it++;
		}
	}


	for (auto it = lv_ps.begin(); it != lv_ps.end();) {
		Level_Up lv = *it;

		if (lv.get_duration() <= 0) {
			it = lv_ps.erase(it);
			lv.destroy();
			continue;
		}
		else
		{
			it++;
		}
	}

	for (auto it = m_cross.begin(); it != m_cross.end();) {
		Cross* cr = *it;
		if (cr->used == true) {
			it = m_cross.erase(it);
			Mix_PlayChannel(-1, m_player_eat_cross, 0);
			cr->destroy();
			b2world->DestroyBody(cr->rigidbody);
			delete cr;
			continue;
		}
		else
		{
			it++;
		}
	}

	for (auto it = goldenorbs.begin(); it != goldenorbs.end();) {
		GoldenOrb* g = *it;
		if (g->erase_flag == true) {
			it = goldenorbs.erase(it);
			Mix_PlayChannel(-1, m_player_eat_golden_orb, 0);
			g->destroy();
			b2world->DestroyBody(g->rigidbody);
			delete g;
			continue;
		}
		else{
		}
		++it;
	}

	for (auto it = m_chest.begin(); it != m_chest.end();) {
		Chest* g = *it;
		if (g->erase_flag == true) {
			it = m_chest.erase(it);
	
			g->destroy();
			b2world->DestroyBody(g->rigidbody);
			delete g;
			continue;
		}
		else {
		}
		++it;
	}

	for (auto it = m_pillar.begin(); it != m_pillar.end();) {
		Pillar* g = *it;
		if (g->erase_flag == true) {
			it = m_pillar.erase(it);

			g->destroy();
			b2world->DestroyBody(g->rigidbody);
			delete g;
			continue;
		}
		else {
		}
		++it;
	}

	for (auto it = m_shield_ps.begin(); it != m_shield_ps.end();) {
		Shield_P* shield_p = *it;

		if (shield_p->shouldErase || shield_p->get_duration() <= 0) {
			it = m_shield_ps.erase(it);
			Mix_PlayChannel(-1, m_player_shield_break, 0);
			shield_p->destroy();
			b2world->DestroyBody(shield_p->rigidbody);
			delete shield_p;
			continue;
		}

		it++;
	}

	for (auto it = stones.begin(); it != stones.end();) {
		Stone* sstone = *it;

		if (abs(m_player.get_position().x - sstone->get_position().x) <= sstone->near_distance) {
		
			if (sstone->idle) {
					sstone->falling = 1;
					sstone->idle = 0;
			}
		}
		it++;
	}

	if (m_shield_ps.size() == 0) m_player.set_shield(false);


	for (auto& hp : m_hps)
		hp->update(elapsed_ms*m_current_speed);

	if (game_level == 1) m_npc.update(elapsed_ms);


	// Updating all entities, making the enemy and npc
	// faster based on current
	m_player.update(elapsed_ms * m_current_speed);
	m_arrow.set_position(m_player.get_position());
	// checking player - ground collisions

	for (auto it = m_enemies.begin(); it != m_enemies.end();) {

		Enemy* enemy = *it;

		State state = decision_tree.start.front()->judge(enemy);
		switch (state)
		{
		case condition:
			break;
		case attack:
			enemy->attack_player(true);
			if (enemy->dragon && Fire_e::cd_count == 15) {
				Fire_e* df = new Fire_e;
				vec2 position = enemy->get_position();
				position.x += enemy->physic.face_right ? 860 : -860;
				position.y += 200.f;
				vec2 facing = enemy->physic.face_right ? vec2{1, 0} : vec2{0, 1};
				if (!enemy->fly) {
					df->init(position, facing);
					dragon_fires.emplace_back(df);

                 /*
				AABB* df_ab = coll.Abinit(df->vertex[2], df->vertex[0], dragonfire_a);
				coll.addverindices(df_ab, df->indicess);
				coll.addvertex(df_ab, df->vertex);
				df_ab->df = df;
				aabbs.emplace_back(df_ab);
				*/

				df->setDef();
				df->rigidbody = b2world->CreateBody(&df->dragonFireBodyDef);
				df->rigidbody->CreateFixture(&df->dragonFireFixtureDef);
				df->rigidbody->SetUserData(df);

				}
			}
			if (Thunder_e::cd_count == 9 && enemy->fly && enemy->dragon) {
				Thunder_e* te = new Thunder_e;
				vec2 facing = enemy->physic.face_right ? vec2{ 1, 0 } : vec2{ 0, 1 };
				te->init({ m_player.get_position().x, m_player.get_position().y - 50.f }, facing);
				dragon_thunder.emplace_back(te);

				te->setDef();
				te->rigidbody = b2world->CreateBody(&te->dragonThunderBodyDef);
				te->rigidbody->CreateFixture(&te->dragonThunderFixtureDef);
				te->rigidbody->SetUserData(te);

			}
			break;
		case die: {
			m_player.add_exp(enemy->exp);
			it = m_enemies.erase(it);

			
			int index = 0;
			
			for (int i = 0; i < aabbs.size(); i++) {
				if (aabbs[i]->enemy == enemy) {
					delete aabbs[i];
					index = i;
					break;
				}
			}
			aabbs.erase(aabbs.begin() + index);
			
			for (int i = 0; i < m_hps.size(); i++) {
				if (m_hps[i] == enemy->healthpoint) {
					m_hps[i]->destroy();
					delete m_hps[i];
					index = i;
					break;
				}
			}
			m_hps.erase(m_hps.begin() + index);
			enemy->destroy();
			b2world->DestroyBody(enemy->rigidbody);
			delete enemy;
			continue;
			break; }
		case left:
			enemy->setRun(1);
			break;
		case right:
			enemy->setRun(-1);
			break;
		case idle:
			break;
		case enrage:
			enemy->rage = true;
			//enemy->speed_add += 80;
			enemy->max_step += 2.f;
			enemy->count = enemy->count / 2;
			break;
		case jump:
			enemy->setJump();
			break;
		default:
			break;
		}

		++it;
		//enemy->set_height_limit(game_level>0? 1500.f:1600.f);
		enemy->set_height_limit(1600.f);
		//if (game_level == 2) { enemy->set_height_limit(1000.f); }
		enemy->update(elapsed_ms * m_current_speed);
		
		
	}

	m_helper.update(elapsed_ms * m_current_speed);
	for (auto& fire : m_fire) {
		fire.update(elapsed_ms * m_current_speed);

		if (game_level == 0 && fire.collided == true ) {
			if (!m_player.get_casting()) {
				if (!fireball.fireball_status())
					fireball.set_position(fire.get_position());
				fireball.show_fireball(true);
			}
			fire.collided = false;

		}

	}
	fireball.update(elapsed_ms* m_current_speed);

	b2world->Step(timeStep, 8, 3);

	/*
	if (testBall) {
		b2Vec2 position = testBall->GetPosition();
		//float32 angle = ballBody->GetAngle();
		printf("%4.2f %4.2f \n", position.x, position.y);
	}*/


	//fireball.set_position({ fireball.get_position().x, 1650 - ballBody->GetPosition().y});
	//printf("%f, %f & ballbdoy =  %f, %f \n", fireball.get_position().x, fireball.get_position().y, ballBody->GetPosition().x, ballBody->GetPosition().y);

	m_keyui.update(elapsed_ms);
	flareIcon.update(elapsed_ms);
	shieldIcon.update(elapsed_ms);
	potionIcon.update(elapsed_ms);
	soulIcon.update(elapsed_ms);
	fullblood.update(elapsed_ms);

	soul_chain.check_enough_lv();
	if (!soul_chain.activated) {
		Enemy* e = NULL;
		vec2 p_p = m_player.get_position();
		for (auto& enemy : m_enemies) {
			vec2 p_e = enemy->get_position();
			float distance_x = p_e.x - p_p.x;
			float distance_y = p_e.y - p_p.y;
			vec2 d = vec2{distance_x, distance_y};
			float distance = dot(d, d);
			if (distance <= 1000000.f) {
				if (e == NULL) e = enemy;
				else if (dot(vec2{ e->get_position().x - p_p.x, e->get_position().y - p_p.y }, vec2{ e->get_position().x - p_p.x, e->get_position().y - p_p.y }) > distance) {
					e = enemy;
				}
			}
		}
		if (e != NULL) {
			soul_chain.set_enemy(e);
			soul_chain.in_range = true;
		}
		else soul_chain.in_range = false;
	}
	else soul_chain.update(elapsed_ms);
	for (auto& attack : m_attack_ps)
		attack->update(elapsed_ms * m_current_speed);
	for (auto& f : dragon_fires)
		f->update(elapsed_ms * m_current_speed);
	for (auto& f : dragon_thunder)
		f->update(elapsed_ms * m_current_speed);
	for (auto& shield : m_shield_ps)
		shield->update(elapsed_ms * m_current_speed);
	for (auto& flare : m_flare_ps)
		flare->update(elapsed_ms* m_current_speed);
	for (auto& lv : lv_ps)
		lv.update(elapsed_ms* m_current_speed);
	for (auto& o : jumporbs)
		o->update(elapsed_ms * m_current_speed);
	for (auto& o : goldenorbs)
		o->update(elapsed_ms * m_current_speed);
	for (auto& c : m_chest) {
		c->update(elapsed_ms * m_current_speed);
		if (c->lockLife <= 0) {
			c->erase_flag = true;
			GoldenOrb* o1 = c->broken();
			goldenorbs.emplace_back(o1);
			o1->rigidbody = b2world->CreateBody(&o1->goldenBodyDef);
			o1->rigidbody->CreateFixture(&o1->goldenFixtureDef);
			o1->rigidbody->SetUserData(o1);
		}
	}
	for (auto& p : m_pillar)
		p->update(elapsed_ms * m_current_speed);
	for (auto& s : stones)
		s->update(elapsed_ms * m_current_speed);
	for (auto& m : m_mace)
		m->update(elapsed_ms * m_current_speed);
	for (auto& v : m_venom)
		v->update(elapsed_ms * m_current_speed);
	for (auto& s : m_swamp)
		s->update(elapsed_ms * m_current_speed);
	
	Level_Up lv = m_player.level_u();
	if (lv.player) lv_ps.emplace_back(lv);

	if (m_player.deadOrNot() && !checked) {
		checked = 1;

		Mix_PlayChannel(-1, m_player_dead_sound, 0);
		m_frontGround.set_player_dead();
	}


	if (!m_player.is_alive() &&
		m_frontGround.get_player_dead_time() > 5) {
		int w, h;
		glfwGetWindowSize(m_window, &w, &h);
		//m_player.destroy();
		clear();
		spawn();
	}



	return true;
}

void World::collision_check() {
	
	int erase_counts = 0;
	std::vector<AABB*> aass = aabbs;

	for (auto it = aabbs.begin(); it != aabbs.end();) {

		AABB* element = *it;

		if (element->type == player_a) {

			

			element->vextex[0].x = m_player.get_position().x + 10;
			element->vextex[0].y = m_player.get_position().y + 10;

			element->vextex[1].x = m_player.get_position().x + 10 ;
			element->vextex[1].y = m_player.get_position().y - 10;

			element->vextex[2].x = m_player.get_position().x - 10;
			element->vextex[2].y = m_player.get_position().y - 10;

			element->vextex[3].x = m_player.get_position().x - 10;
			element->vextex[3].y = m_player.get_position().y + 10;


			element->max->var.x = m_player.get_position().x + 10;
			element->min->var.x = m_player.get_position().x - 10;
			element->max->var.y = m_player.get_position().y + 10;
			element->min->var.y = m_player.get_position().y - 10;
			

		}

		else  if (element->type == enemy_a) {

			
			if (element->enemy == NULL) {
				it = aabbs.erase(it);
				continue;
				//delete element;
			}
			else {

				// judege if it is worlf
				if (Wolf* v = dynamic_cast<Wolf*>(element->enemy)) {

					element->vextex[0].x = element->enemy->get_position().x + 20;
					element->vextex[0].y = element->enemy->get_position().y + 100;

					element->vextex[1].x = element->enemy->get_position().x + 20;
					element->vextex[1].y = element->enemy->get_position().y - 20;

					element->vextex[2].x = element->enemy->get_position().x - 100;
					element->vextex[2].y = element->enemy->get_position().y - 20;

					element->vextex[3].x = element->enemy->get_position().x - 100;
					element->vextex[3].y = element->enemy->get_position().y + 100;


					element->max->var.x = element->enemy->get_position().x + 20;
					element->min->var.x = element->enemy->get_position().x - 100;
					element->max->var.y = element->enemy->get_position().y + 100;
					element->min->var.y = element->enemy->get_position().y - 20;

					element->enemy->flared = false;
					element->enemy->echecked = false;



				}else if (Dragon* v = dynamic_cast<Dragon*>(element->enemy)) {

					element->vextex[0].x = element->enemy->get_position().x + 20;
					element->vextex[0].y = element->enemy->get_position().y + 100;

					element->vextex[1].x = element->enemy->get_position().x + 20;
					element->vextex[1].y = element->enemy->get_position().y - 20;

					element->vextex[2].x = element->enemy->get_position().x - 100;
					element->vextex[2].y = element->enemy->get_position().y - 20;

					element->vextex[3].x = element->enemy->get_position().x - 100;
					element->vextex[3].y = element->enemy->get_position().y + 100;


					element->max->var.x = element->enemy->get_position().x + 300;
					element->min->var.x = element->enemy->get_position().x - 300;
					element->max->var.y = element->enemy->get_position().y + 300;
					element->min->var.y = element->enemy->get_position().y - 300;

					element->enemy->flared = false;
					element->enemy->echecked = false;



				}
				else {

				element->vextex[0].x = element->enemy->get_position().x + 60;
				element->vextex[0].x = element->enemy->get_position().y + 60;

				element->vextex[1].x = element->enemy->get_position().x + 60;
				element->vextex[1].x = element->enemy->get_position().y - 60;

				element->vextex[2].x = element->enemy->get_position().x - 60;
				element->vextex[2].x = element->enemy->get_position().y - 60;

				element->vextex[3].x = element->enemy->get_position().x - 60;
				element->vextex[3].x = element->enemy->get_position().y + 60;


				element->max->var.x = element->enemy->get_position().x + 60;
				element->min->var.x = element->enemy->get_position().x - 60;
				element->max->var.y = element->enemy->get_position().y + 105;
				element->min->var.y = element->enemy->get_position().y - 105;

				element->enemy->flared = false;
				element->enemy->echecked = false;
				}
			}


		
		}
		/*
		else if (element->type == fire_a) {

		
			element->vextex[0].x = element->fire->get_position().x + 5;
			element->vextex[0].x = element->fire->get_position().y + 5;
			element->vextex[1].x = element->fire->get_position().x + 5;
			element->vextex[1].x = element->fire->get_position().y - 5;
			element->vextex[2].x = element->fire->get_position().x - 5;
			element->vextex[2].x = element->fire->get_position().y - 5;
			element->vextex[3].x = element->fire->get_position().x - 5;
			element->vextex[3].x = element->fire->get_position().y + 5;


			element->max->var.x = element->fire->get_position().x + 15;
			element->min->var.x = element->fire->get_position().x - 15;
			element->max->var.y = element->fire->get_position().y + 15;
			element->min->var.y = element->fire->get_position().y - 15;

			
			element->fire->collided = 0;
		}*/

		/*
		else if (element->type == venom_a) {
		element->vextex[0].x = element->venom->get_position().x + 40;
		element->vextex[0].x = element->venom->get_position().y + 40;
		element->vextex[1].x = element->venom->get_position().x + 40;
		element->vextex[1].x = element->venom->get_position().y - 40;
		element->vextex[2].x = element->venom->get_position().x - 40;
		element->vextex[2].x = element->venom->get_position().y - 40;
		element->vextex[3].x = element->venom->get_position().x - 40;
		element->vextex[3].x = element->venom->get_position().y + 40;


		element->max->var.x = element->venom->get_position().x + 40;
		element->min->var.x = element->venom->get_position().x - 40;
		element->max->var.y = element->venom->get_position().y + 40;
		element->min->var.y = element->venom->get_position().y - 40;
		}*/
		/*
		else if (element->type == mace_a) {
		element->vextex[0].x = element->mace->get_head_b_position().x + 150;
		element->vextex[0].y = element->mace->get_head_b_position().y + 50;
		element->vextex[1].x = element->mace->get_head_a_position().x - 150;
		element->vextex[1].y = element->mace->get_head_a_position().y - 50;

		element->max->var.x = (element->mace->get_head_b_position().x + element->mace->get_head_a_position().x) / 2 + 85;
		element->min->var.x = (element->mace->get_head_b_position().x + element->mace->get_head_a_position().x) / 2 - 75;
		element->max->var.y = (element->mace->get_head_b_position().y + element->mace->get_head_a_position().y) / 2 + 85;
		element->min->var.y = (element->mace->get_head_b_position().y + element->mace->get_head_a_position().y) / 2 - 70;
		}*/
		/*
		else if (element->type == cross_a) {

		
			
			if (element->cross == NULL) {
				it = aabbs.erase(it);
				continue;
				//delete element;
			}
			else {

				element->vextex[0].x = element->cross->get_position().x + 100;
				element->vextex[0].y = element->cross->get_position().y + 100;
				element->vextex[1].x = element->cross->get_position().x + 100;
				element->vextex[1].y = element->cross->get_position().y - 100;
				element->vextex[2].x = element->cross->get_position().x - 100;
				element->vextex[2].y = element->cross->get_position().y - 100;
				element->vextex[3].x = element->cross->get_position().x - 100;
				element->vextex[3].y = element->cross->get_position().y + 100;


				element->max->var.x = element->cross->get_position().x + 100;
				element->min->var.x = element->cross->get_position().x - 100;
				element->max->var.y = element->cross->get_position().y + 100;
				element->min->var.y = element->cross->get_position().y - 100;
			}
		}*/
		/*
		else if (element->type == gold_orb_a) {


		     if (element->gold == NULL || element->gold->erase_flag == true) {
				 it = aabbs.erase(it);
				 continue;
				 //delete element;
			 }
			 else {

				 element->vextex[0].x = element->gold->get_position().x + 45;
				 element->vextex[0].y = element->gold->get_position().y + 45;
				 element->vextex[1].x = element->gold->get_position().x + 45;
				 element->vextex[1].y = element->gold->get_position().y - 45;
				 element->vextex[2].x = element->gold->get_position().x - 45;
				 element->vextex[2].y = element->gold->get_position().y - 45;
				 element->vextex[3].x = element->gold->get_position().x - 45;
				 element->vextex[3].y = element->gold->get_position().y + 45;


				 element->max->var.x = element->gold->get_position().x + 45;
				 element->min->var.x = element->gold->get_position().x - 45;
				 element->max->var.y = element->gold->get_position().y + 45;
				 element->min->var.y = element->gold->get_position().y - 45;
			 }

		}*/

		/*
		else if (element->type == attack_a) {

			if (element->attack == NULL) {

				it = aabbs.erase(it);
				continue;
				//delete element;


			}
			else {

				element->vextex[0].x = element->attack->get_position().x + 10;
				element->vextex[0].y = element->attack->get_position().y + 10;
				element->vextex[1].x = element->attack->get_position().x + 10;
				element->vextex[1].y = element->attack->get_position().y - 10;
				element->vextex[2].x = element->attack->get_position().x - 10;
				element->vextex[2].y = element->attack->get_position().y - 10;
				element->vextex[3].x = element->attack->get_position().x - 10;
				element->vextex[3].y = element->attack->get_position().y + 10;


				element->max->var.x = element->attack->get_position().x + 10;
				element->min->var.x = element->attack->get_position().x - 10;
				element->max->var.y = element->attack->get_position().y + 10;
				element->min->var.y = element->attack->get_position().y - 10;

			}

		}*/
		/*
		else if (element->type == flare_a) {

		if (element->flare == NULL) {

			it = aabbs.erase(it);
			continue;
			//delete element;


		}
		else {

			element->vextex[0].x = element->flare->get_position().x + 400;
			element->vextex[0].y = element->flare->get_position().y + 400;
			element->vextex[1].x = element->flare->get_position().x + 400;
			element->vextex[1].y = element->flare->get_position().y - 400;
			element->vextex[2].x = element->flare->get_position().x - 400;
			element->vextex[2].y = element->flare->get_position().y - 400;
			element->vextex[3].x = element->flare->get_position().x - 400;
			element->vextex[3].y = element->flare->get_position().y + 400;


			element->max->var.x = element->flare->get_position().x + 400;
			element->min->var.x = element->flare->get_position().x - 400;
			element->max->var.y = element->flare->get_position().y + 400;
			element->min->var.y = element->flare->get_position().y - 400;

		}

		}*/


        
		else if (element->type == ground_a) {
		if (element->ground == NULL) {

			it = aabbs.erase(it);
			continue;
			//delete element;


		}

		element->vextex[0].x = element->ground->get_position().x + 85;
		element->vextex[0].y = element->ground->get_position().y + 120;
		element->vextex[1].x = element->ground->get_position().x + 85;
		element->vextex[1].y = element->ground->get_position().y - 150;
		element->vextex[2].x = element->ground->get_position().x - 85;
		element->vextex[2].y = element->ground->get_position().y - 150;
		element->vextex[3].x = element->ground->get_position().x - 85;
		element->vextex[3].y = element->ground->get_position().y + 150;


		element->max->var.x = element->ground->get_position().x  + 85;
		element->min->var.x = element->ground->get_position().x  - 85;
		element->max->var.y = element->ground->get_position().y  + 120;
		element->min->var.y = element->ground->get_position().y  - 150;

		element->ground->checked = false;

		}
		
		
		/*
		else if (element->type == jump_orb_a) {


		if (element->orb == NULL) {

			it = aabbs.erase(it);
			continue;
			//delete element;


		}
		
		element->vextex[0].x = element->orb->get_position().x + 50;
		element->vextex[0].y = element->orb->get_position().y + 50;
		element->vextex[1].x = element->orb->get_position().x + 50;
		element->vextex[1].y = element->orb->get_position().y - 50;
		element->vextex[2].x = element->orb->get_position().x - 50;
		element->vextex[2].y = element->orb->get_position().y - 50;
		element->vextex[3].x = element->orb->get_position().x - 50;
		element->vextex[3].y = element->orb->get_position().y + 50;


		element->max->var.x = element->orb->get_position().x + 50;
		element->min->var.x = element->orb->get_position().x - 50;
		element->max->var.y = element->orb->get_position().y + 50;
		element->min->var.y = element->orb->get_position().y - 50;

		element->orb->collided = 0;

       
       }
	   */
	   /*
		else if (element->type == shield_a) {

		if (element->shield == NULL) {

			it = aabbs.erase(it);
			continue;
			//delete element;


		}
		else {

			element->vextex[0].x = element->shield->get_position().x + 100;
			element->vextex[0].y = element->shield->get_position().y + 100;
			element->vextex[1].x = element->shield->get_position().x + 100;
			element->vextex[1].y = element->shield->get_position().y - 100;
			element->vextex[2].x = element->shield->get_position().x - 100;
			element->vextex[2].y = element->shield->get_position().y - 100;
			element->vextex[3].x = element->shield->get_position().x - 100;
			element->vextex[3].y = element->shield->get_position().y - 100;


			element->max->var.x = element->shield->get_position().x + 100;
			element->min->var.x = element->shield->get_position().x - 100;
			element->max->var.y = element->shield->get_position().y + 100;
			element->min->var.y = element->shield->get_position().y - 100;

		}

		}*/

		/*
		else if (element->type == fireball_a) {


			if (element->fireball == NULL) {
			
				it = aabbs.erase(it);
				continue;
				//delete element;
			
			}

			else {

				element->vextex[0].x = element->fireball->get_position().x + 50;
				element->vextex[0].y = element->fireball->get_position().y + 50;
				element->vextex[1].x = element->fireball->get_position().x + 50;
				element->vextex[1].y = element->fireball->get_position().y - 50;
				element->vextex[2].x = element->fireball->get_position().x - 50;
				element->vextex[2].y = element->fireball->get_position().y - 50;
				element->vextex[3].x = element->fireball->get_position().x - 50;
				element->vextex[3].y = element->fireball->get_position().y + 50;


				element->max->var.x = element->fireball->get_position().x + 50;
				element->min->var.x = element->fireball->get_position().x - 50;
				element->max->var.y = element->fireball->get_position().y + 50;
				element->min->var.y = element->fireball->get_position().y - 50;

			
			}
        }*/
		
		/*
		else if (element->type == stone_a) {
		

		if (element->sstone == NULL) {

			it = aabbs.erase(it);
			continue;
			//delete element;

		}

		else {

			element->vextex[0].x = element->sstone->get_position().x + 50;
			element->vextex[0].y = element->sstone->get_position().y + 50;
			element->vextex[1].x = element->sstone->get_position().x + 50;
			element->vextex[1].y = element->sstone->get_position().y - 50;
			element->vextex[2].x = element->sstone->get_position().x - 50;
			element->vextex[2].y = element->sstone->get_position().y - 50;
			element->vextex[3].x = element->sstone->get_position().x - 50;
			element->vextex[3].y = element->sstone->get_position().y + 50;


			element->max->var.x = element->sstone->get_position().x + 100;
			element->min->var.x = element->sstone->get_position().x - 100;
			element->max->var.y = element->sstone->get_position().y + 1000;
			element->min->var.y = element->sstone->get_position().y - 100;


		}
		}*/
		/*
		else if (element->type == dragonfire_a) {

			if (element->df == NULL) {
				it = aabbs.erase(it);
				continue;
			}
			else {
			
				element->vextex[0].x = element->df->get_position().x + 300;
				element->vextex[0].y = element->df->get_position().y + 100;
				element->vextex[1].x = element->df->get_position().x + 50;
				element->vextex[1].y = element->df->get_position().y - 50;
				element->vextex[2].x = element->df->get_position().x - 300;
				element->vextex[2].y = element->df->get_position().y - 100;
				element->vextex[3].x = element->df->get_position().x - 50;
				element->vextex[3].y = element->df->get_position().y + 50;


				element->max->var.x = element->df->get_position().x + 300;
				element->min->var.x = element->df->get_position().x - 300;
				element->max->var.y = element->df->get_position().y + 100;
				element->min->var.y = element->df->get_position().y - 100;
			
			}
		     
        }*/
		it++;

	}


	int i = 0;
	coll.SweepAndPrune(aabbs, &countDown);
	int j = 0;
}

bool World::WorldSave()
{
	SaveLoad sl;
	//save player
	sPlayer*  p1 = m_player.getInfo();

	//save enemy
	std::vector<sEnemy*> es;
	for (auto enemy : m_enemies) {
		sEnemy* se = enemy->getInfo();
		es.emplace_back(se);
	}

	//save golden orb
	std::vector<sGoldenOrb*> go;
	for (auto golden : goldenorbs) {
		sGoldenOrb* g = golden->getInfo();
		go.emplace_back(g);
	}

	//save cross
	std::vector<sCross*> sc;
	for (auto cro : m_cross) {
		sCross* c = cro->getInfo();
		sc.emplace_back(c);
	}

	//save quest
	sQuestManager* qm = quest_manager.getInfo();

	//save Item
	std::vector<sItem*> it;
	for (auto i : m_player.items) {
		sItem* si = i->getInfo();
		it.emplace_back(si);
	}

	//save chest
	std::vector<sChest*> cc;
	for (auto i : m_chest) {
		sChest* c = i->getInfo();
		cc.emplace_back(c);
	}

	std::vector<sPillar*> sp;
	for (auto i : m_pillar) {
		sPillar* p = i->getInfo();
		sp.emplace_back(p);
	}

	sl.save(p1, es, go, sc, game_level, qm, it, cc, sp);

	delete p1;
	for (auto s : es) {
		delete s;
	}
	for (auto g : go) {
		delete g;
	}
	for (auto c : sc) {
		delete c;
	}
	for (auto i : it) {
		delete i;
	}
	for (auto c : cc) {
		delete c;
	}
	for (auto p : sp) {
		delete p;
	}
	delete qm;
	return 1;
}

bool World::WorldLoad()
{
	SaveLoad sl;

	sPlayer* Play = new sPlayer[1];
	sEnemy* Enemys = new sEnemy[28];
	int enemyNo;
	sGoldenOrb* gorb = new sGoldenOrb[16];
	int orbNo;
	sCross* cross = new sCross[8];
	int crossNo;
	sQuestManager* qm = new sQuestManager[1];
	int itemNo;
	sItem* it = new sItem[5];
	int chestNo;
	sChest* cc = new sChest[5];
	int pillarNo;
	sPillar* sp = new sPillar[5];
	previous_level = game_level;
	sl.load(&m_player, Enemys, &enemyNo, gorb, &orbNo, cross, &crossNo, &game_level, &quest_manager, it, &itemNo, cc, &chestNo, sp, &pillarNo);
	

	if (game_level != previous_level) {
		delete[] Play;
		Play = m_player.getInfo();
		scene_change();
		m_player.setInfo(&Play[0]);
		//previous_level = game_level;
	}



	/*if (quest_manager.inserted1 && quest_manager.)
	{
		quest_manager.insert_quest();
	}*/
	/*for (int i = quest_manager.current; i < quest_manager.quests.size(); i++)
	{
		quest_manager.quests[i].completed = false;
	}
*/
	/*quest_manager.quests[quest_manager.current].completed = false;*/
	
	if (quest_manager.current == 4) {
		int remain = quest_manager.quests.size() - quest_manager.current;
		for (int i = 0; i < remain; i++) {
			quest_manager.quests.pop_back();
		}
	}
	else if (quest_manager.current < 4) {
		for (int i = quest_manager.current; i < 4; i++) {
			quest_manager.quests[i].completed = false;
		}
	}
	else {
		for (int i = quest_manager.current; i < quest_manager.quests.size(); i++) {
			quest_manager.quests[i].completed = false;
		}
	}

	for (auto enemy : m_enemies) {
		enemy->healthpoint->destroy();
		delete enemy->healthpoint;

		
		int index = 0;
		for (int i = 0; i < aabbs.size(); i++) {
			if (aabbs[i]->enemy == enemy) {
				index = i;
				delete aabbs[i];
			}
		}
		aabbs.erase(aabbs.begin() + index);
		enemy->destroy();
		b2world->DestroyBody(enemy->rigidbody);
		delete enemy;
	}

	m_player.healthp->destroy();
	delete m_player.healthp;

	m_hps.clear();
	m_enemies.clear();

	for (auto gorb : goldenorbs) {
		gorb->erase_flag = 1;
		gorb->destroy();

		/*
		int indexg = 0;
		for (int i = 0; i < aabbs.size(); i++) {
			if (aabbs[i]->gold == gorb) {
				indexg = i;
				delete aabbs[i];
			}
		}
		aabbs.erase(aabbs.begin() + indexg);
		*/
		b2world->DestroyBody(gorb->rigidbody);
		delete gorb;
	}

	goldenorbs.clear();

	for (auto cross : m_cross) {
		cross->used = 1;
		cross->destroy();

		/*
		int indexc = 0;
		for (int i = 0; i < aabbs.size(); i++) {
			if (aabbs[i]->cross == cross) {
				indexc = i;
				delete aabbs[i];
			}
		}
		aabbs.erase(aabbs.begin() + indexc);*/
		b2world->DestroyBody(cross->rigidbody);
		delete cross;
	}

	m_cross.clear();

	for (auto che : m_chest) {
		che->erase_flag = true;
		che->destroy();
		b2world->DestroyBody(che->rigidbody);
		delete che;
	}

	m_chest.clear();

	for (auto p : m_pillar) {
		p->erase_flag = true;
		p->destroy();
		b2world->DestroyBody(p->rigidbody);
		delete p;
	}

	m_pillar.clear();

	HealthPoints* hp_player = new HealthPoints;
	hp_player->init(m_player.get_position(), { 1,0 }, m_player.get_hp(), 10);
	m_player.setHPbar(hp_player);
	m_hps.emplace_back(hp_player);



	for (int i = 0; i < crossNo; i++) {
		Cross* cross1 = new Cross;
		if (cross1->init()) {
			cross1->setInfo(&cross[i]);
			cross1->set_vertex_position();
			m_cross.emplace_back(cross1);


			
			cross1->setDef();
			cross1->rigidbody = b2world->CreateBody(&cross1->crossBodyDef);
			cross1->rigidbody->CreateFixture(&cross1->crossFixtureDef);
			//cross1->rigidbody->CreateFixture(&cross1->crossFixtureDef1);
			//cross1->rigidbody->CreateFixture(&cross1->crossFixtureDef2);
			//cross1->rigidbody->CreateFixture(&cross1->crossFixtureDef3);
			//	cross1->rigidbody->CreateFixture(&cross1->crossFixtureDef4);
			cross1->rigidbody->SetUserData(cross1);

			/*
			vec2 min = { cross1->vertices[0].x,cross1->vertices[2].y };
			vec2 max = { cross1->vertices[6].x,cross1->vertices[8].y };

			AABB* cross_ab = coll.Abinit(cross1->vertices[2], cross1->vertices[0], cross_a);
			coll.addverindices(cross_ab, cross1->indicess);
			coll.addvertex(cross_ab, cross1->vertices);
			cross_ab->cross = cross1;
			aabbs.emplace_back(cross_ab);*/
		}
	}


	for (int i = 0; i < orbNo; i++) {
		GoldenOrb *o0 = new GoldenOrb;
		o0->init({ -2700, 1600 });
		o0->draw_state = 1;
		o0->rigidbody = b2world->CreateBody(&o0->goldenBodyDef);
		o0->rigidbody->CreateFixture(&o0->goldenFixtureDef);
		o0->rigidbody->SetUserData(o0);
		o0->setInfo(&gorb[i]);
		goldenorbs.emplace_back(o0);

		/*
		AABB* gold_ab0 = coll.Abinit(o0->vertex[2], o0->vertex[0], gold_orb_a);
		coll.addverindices(gold_ab0, o0->indicess);
		coll.addvertex(gold_ab0, o0->vertex);
		gold_ab0->gold = o0;
		aabbs.emplace_back(gold_ab0);
		*/
	}

	for (int i = 0; i < chestNo; i++) {
		Chest* chest1 = new Chest;
		chest1->init({ -4750,1100 });
		m_chest.emplace_back(chest1);

		chest1->setInfo(&cc[i]);

		chest1->setDef();
		chest1->rigidbody = b2world->CreateBody(&chest1->chestBodyDef);
		chest1->rigidbody->CreateFixture(&chest1->chestFixtureDef);
		chest1->rigidbody->SetUserData(chest1);
		goldenorbs[0]->draw_state = false;
	}

	for (int i = 0; i < 1; i++) {

		if (pillarNo == 2) {
			Pillar* pillar1 = new Pillar;
			pillar1->init({ 2000,80 });
			m_pillar.emplace_back(pillar1);


			pillar1->setDef();
			pillar1->rigidbody = b2world->CreateBody(&pillar1->pillarBodyDef);
			pillar1->rigidbody->CreateFixture(&pillar1->pillarFixtureDef);
			pillar1->rigidbody->SetUserData(pillar1);
			pillar1->setInfo(&sp[0]);

			Pillar* pillar2 = new Pillar;
			pillar2->init({ 3400,740 });
			m_pillar.emplace_back(pillar2);

			pillar2->setDef();
			pillar2->rigidbody = b2world->CreateBody(&pillar2->pillarBodyDef);
			pillar2->rigidbody->CreateFixture(&pillar2->pillarFixtureDef);
			pillar2->rigidbody->SetUserData(pillar2);
			pillar2->setInfo(&sp[1]);
			break;
		}
		else if (pillarNo == 1) {
			Pillar* pillar2 = new Pillar;
			pillar2->init({ 3400,740 });
			m_pillar.emplace_back(pillar2);

			pillar2->setDef();
			pillar2->rigidbody = b2world->CreateBody(&pillar2->pillarBodyDef);
			pillar2->rigidbody->CreateFixture(&pillar2->pillarFixtureDef);
			pillar2->rigidbody->SetUserData(pillar2);
			pillar2->setInfo(&sp[0]);
		}
	}

	for (int i = 0; i < itemNo; i++) {
		m_player.items[i]->setInfo(&it[i]);
	}

	for (int i = 0; i < enemyNo; i++) {

		switch (Enemys[i].type)
		{
		case 0: {
			Skull* enemy = new Skull;
			HealthPoints* hp_enemy = new HealthPoints;
			if (enemy->newInit(&m_player, &m_fire, hp_enemy))
			{

				enemy->rigidbody = b2world->CreateBody(&(enemy->skullBodyDef));
				enemy->rigidbody->CreateFixture(&(enemy->skullFixtureDef));
				enemy->rigidbody->SetUserData(enemy);

				enemy->setInfo(&Enemys[i]);
				m_enemies.emplace_back(enemy);
				hp_enemy->init(enemy->get_position(), { 1,0 }, enemy->get_hp(), enemy->largestHP);
				m_hps.emplace_back(hp_enemy);

						
				AABB* skull_ab = coll.Abinit(enemy->vertex[2], enemy->vertex[0], enemy_a);
				coll.addverindices(skull_ab, enemy->indicess);
				coll.addvertex(skull_ab, enemy->vertex);
				skull_ab->enemy = enemy;
				aabbs.emplace_back(skull_ab);



			}

			break;
		}
		case 1: {
			Viking* viking0 = new Viking;
			HealthPoints* hp_viking = new HealthPoints;
			if (viking0->newInit(&m_player, &m_fire, hp_viking))
			{
				viking0->rigidbody = b2world->CreateBody(&(viking0->vikingBodyDef));
				viking0->rigidbody->CreateFixture(&(viking0->vikingFixtureDef));
				viking0->rigidbody->SetUserData(viking0);

				viking0->setInfo(&Enemys[i]);
				m_enemies.emplace_back(viking0);
				hp_viking->init(viking0->get_position(), { 1,0 }, viking0->get_hp(), viking0->largestHP);
				m_hps.emplace_back(hp_viking);

				
				AABB* viking_ab = coll.Abinit(viking0->vertex[2], viking0->vertex[0], enemy_a);
				coll.addverindices(viking_ab, viking0->indicess);
				coll.addvertex(viking_ab, viking0->vertex);
				viking_ab->enemy = viking0;
				aabbs.emplace_back(viking_ab);
				
			}

			break;
		}
		case 2: {
			Knight* knight = new Knight;
			HealthPoints* hp_knight = new HealthPoints;
			if (knight->newInit(&m_player, &m_fire, hp_knight))
			{
				knight->rigidbody = b2world->CreateBody(&(knight->knightBodyDef));
				knight->rigidbody->CreateFixture(&(knight->knightFixtureDef));
				knight->rigidbody->SetUserData(knight);


				knight->setInfo(&Enemys[i]);
				m_enemies.emplace_back(knight);
				hp_knight->init(knight->get_position(), { 1,0 }, knight->get_hp(), knight->largestHP);
				m_hps.emplace_back(hp_knight);

				
				AABB* knight_ab = coll.Abinit(knight->vertex[2], knight->vertex[0], enemy_a);
				coll.addverindices(knight_ab, knight->indicess);
				coll.addvertex(knight_ab, knight->vertex);
				knight_ab->enemy = knight;
				aabbs.emplace_back(knight_ab);
				

			}

			break;
		}
		case 3: {
			Wolf* wolf = new Wolf;
			HealthPoints* hp_wolf = new HealthPoints;
			if (wolf->newInit(&m_player, &m_fire, hp_wolf))
			{
				wolf->rigidbody = b2world->CreateBody(&wolf->wolfBodyDef);
				wolf->rigidbody->CreateFixture(&wolf->wolfFixtureDef);
				wolf->rigidbody->SetUserData(wolf);

				wolf->setInfo(&Enemys[i]);
				m_enemies.emplace_back(wolf);
				hp_wolf->init(wolf->get_position(), { 1,0 }, wolf->get_hp(), wolf->largestHP);
				m_hps.emplace_back(hp_wolf);

				
				AABB* wolf_ab = coll.Abinit(wolf->vertex[2], wolf->vertex[0], enemy_a);
				coll.addverindices(wolf_ab, wolf->indicess);
				coll.addvertex(wolf_ab, wolf->vertex);
				wolf_ab->enemy = wolf;
				aabbs.emplace_back(wolf_ab);
				
			}
			break;
		}
		case 4: {
		
			Dragon* dragon = new Dragon;
			HealthPoints* hp_dragon = new HealthPoints;
			if (dragon->newInit(&m_player, &m_fire, hp_dragon))
			{
				dragon->rigidbody = b2world->CreateBody(&(dragon->dragonBodyDef));
				dragon->rigidbody->CreateFixture(&(dragon->dragonFixtureDef));
				dragon->rigidbody->SetUserData(dragon);


				dragon->setInfo(&Enemys[i]);
				m_enemies.emplace_back(dragon);
				hp_dragon->init(dragon->get_position(), { 1,0 }, dragon->get_hp(), dragon->largestHP);
				m_hps.emplace_back(hp_dragon);

				
				AABB* dragon_ab = coll.Abinit(dragon->vertex[2], dragon->vertex[0], enemy_a);
				coll.addverindices(dragon_ab, dragon->indicess);
				coll.addvertex(dragon_ab, dragon->vertex);
				dragon_ab->enemy = dragon;
				aabbs.emplace_back(dragon_ab);
				
			}
			break;
		}

		}
	}

	delete[] Play;
	delete[] Enemys;
	delete[] gorb;
	delete[] cross;
	delete[] it;
	delete[] cc;

	return 1;
}



// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void World::draw()
{
	// Clearing error buffer
	gl_flush_errors();

	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);

	// Updating window title with points
	std::stringstream title_ss;
	int health = (m_player.get_hp() >= 0) ? m_player.get_hp() : 0;
	title_ss << "Game Level 1 | Player Level: " << m_player.get_level() << " ,   EXP: " << m_player.get_exp() << " ,   HP: " << health << ", FPS: " << fps;
	glfwSetWindowTitle(m_window, title_ss.str().c_str());

	/////////////////////////////////////
	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	//glViewport(m_player.get_position().x-w/2, m_player.get_position().y-h/2, m_player.get_position().x+w/2, m_player.get_position().y+h/2);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = { 0.3f, 0.3f, 0.8f };
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Fake projection matrix, scales with respect to window coordinates
	// PS: 1.f / w in [1][1] is correct.. do you know why ? (:
	float left = m_player.get_position().x - (float)w/2.0;// *-0.5;
	float top = m_player.get_position().y - (float)h / 2.0 - (float)h / 6.0;// plus h/6.0 for moving camera a bit higher
	float right = (float)w/2.0 + m_player.get_position().x;// *0.5;
	float bottom = (float)h / 2.0 + m_player.get_position().y - (float)h / 6.0;// *0.5;
	if (left <= -screen_texs[game_level].width) {    //if camera's left reaches the left boundary of background, lock the movement of camera unless the player move right for a distance
		left = -screen_texs[game_level].width;
		right = -screen_texs[game_level].width + (float)w;
	}
	if (right >= screen_texs[game_level].width) {
		right = screen_texs[game_level].width;
		left = screen_texs[game_level].width - (float)w;
	}
	if (top <= -screen_texs[game_level].height) {
		top = -screen_texs[game_level].height;
		bottom = -screen_texs[game_level].height + (float)h;
	}
	if (bottom >= screen_texs[game_level].height) {
		bottom = screen_texs[game_level].height;
		top = screen_texs[game_level].height - (float)h;
	}

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };
	mat3 rot_2d = {{ 1.f, 1.f, 0 }, { 1.f, 1.f, 0 }, { 0.f, 0.f, 1.f }};
	mat3 trans_2d = { { 1.f, 0.f, 0 }, { 0.f, 1.f, 0 }, { -m_player.get_position().x, -m_player.get_position().y, 1.f } };
	mat3 slow_projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx/5.f, ty/5.f, 1.f } };
	//mat3 view_2D = mul(rot_2d, trans_2d);
	mat3 view_2D = { {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}};



	/////////////////////
	// Truely render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clearing backbuffer
	glViewport(0, 0, w, h);

	//glOrtho(-1.5, 1.5, -1.5, 1.5, -10, 10);
	//glViewport(m_player.get_position().x - w / 2, m_player.get_position().y - h / 2, m_player.get_position().x + w / 2, m_player.get_position().y + h / 2);
	glDepthRange(0, 10);
	glClearColor(0, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (start_show) {
		m_start.draw(mat3({ { -0.1f, 0.f, 0.f },{ 0.f, -0.1f, 0.f },{ 0.f, 0.f, 1.f } }), view_2D);
		glfwSwapBuffers(m_window);
		return;
	}
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	
	glBindTexture(GL_TEXTURE_2D, screen_texs[game_level].id);
	m_backGround.draw(slow_projection_2D, view_2D);
	m_frontGround.draw(projection_2D, view_2D);
	
	
	// Drawing entities
	fireball.draw(projection_2D, view_2D);
	for (auto& flare : m_flare_ps)
		flare->draw(projection_2D, view_2D);
	for (auto& f : dragon_fires)
		f->draw(projection_2D, view_2D);
	for (auto& f : dragon_thunder)
		f->draw(projection_2D, view_2D);
	if (smoke.get_duration() > 0)
		smoke.draw(projection_2D, view_2D);
	for (auto& g : grounds)
		g->draw(projection_2D, view_2D);
	for (auto& enemy : m_enemies)
		enemy->draw(projection_2D, view_2D);
	for (auto& attack : m_attack_ps)
		attack->draw(projection_2D, view_2D);
	for (auto& shield : m_shield_ps)
		shield->draw(projection_2D, view_2D);
	for (auto& fire : m_fire)
		fire.draw(projection_2D, view_2D);
	for (auto& mace : m_mace)
		mace->draw(projection_2D, view_2D);
	for (auto& venom : m_venom)
		venom->draw(projection_2D, view_2D);
	for (auto& swamp : m_swamp)
		swamp->draw(projection_2D, view_2D);
	for (auto& pillar : m_pillar)
		pillar->draw(projection_2D, view_2D);
	for (auto& chest : m_chest)
		chest->draw(projection_2D, view_2D);
	for (auto& cross : m_cross)
		cross->draw(projection_2D, view_2D);
	for (auto& g : goldenorbs)
		g->draw(projection_2D, view_2D);
	for (auto& o : jumporbs)
		o->draw(projection_2D, view_2D);
	for (auto& hp : m_hps)
		hp->draw(projection_2D, view_2D);
	for (auto& sstone : stones)
		sstone->draw(projection_2D, view_2D);
	soul_chain.draw(projection_2D, view_2D);
	
	
	if (game_level == 1) m_npc.draw(projection_2D, view_2D);
	m_player.draw(projection_2D, view_2D);
	quest_manager.draw(projection_2D, view_2D);
	
	if (orb_jumping) m_arrow.draw(projection_2D, view_2D);
	for (auto& lv : lv_ps)
		lv.draw(projection_2D, view_2D);

	fullblood.draw(mat3({ { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ 0.f, 0.f, 1.f } }), view_2D);
	m_keyui.draw(mat3({ { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ -0.6f, 0.8f, 1.f } }), view_2D);
	if (helper_show) m_helper.draw(mat3({ { -0.1f, 0.f, 0.f },{ 0.f, -0.1f, 0.f },{ 0.f, 0.f, 1.f } }), view_2D);
	shieldIcon.draw(mat3({ { 0.75f, 0.f, 0.f },{ 0.f, 0.75f, 0.f },{ -0.82f, -0.9f, 1.f } }), view_2D);
	flareIcon.draw(mat3({ { 0.75f, 0.f, 0.f },{ 0.f, 0.75f, 0.f },{ -0.47f, -0.9f, 1.f } }), view_2D);
	potionIcon.draw(mat3({ { 0.75f, 0.f, 0.f },{ 0.f, 0.75f, 0.f },{ -0.12f, -0.9f, 1.f } }), view_2D);
	soulIcon.draw(mat3({ { 0.75f, 0.f, 0.f },{ 0.f, 0.75f, 0.f },{ 0.23f, -0.9f, 1.f } }), view_2D);


	//////////////////
	// Presenting
	glfwSwapBuffers(m_window);
}

// Should the game be over ?
bool World::is_over()const
{
	return glfwWindowShouldClose(m_window);
}
bool World::spawn_cross() {
	if (game_level == 0)
	{
		
		Cross* cross1 = new Cross;
		Cross* cross2 = new Cross;

		if (cross1->init()) {
			cross1->set_position(vec2{ 0, m_player.get_position().y + 50 });
			cross1->set_vertex_position();
			m_cross.emplace_back(cross1);

			cross1->setDef();
			cross1->rigidbody = b2world->CreateBody(&cross1->crossBodyDef);
			cross1->rigidbody->CreateFixture(&cross1->crossFixtureDef);
		//	cross1->rigidbody->CreateFixture(&cross1->crossFixtureDef1);
		//	cross1->rigidbody->CreateFixture(&cross1->crossFixtureDef2);
		//	cross1->rigidbody->CreateFixture(&cross1->crossFixtureDef3);
		//	cross1->rigidbody->CreateFixture(&cross1->crossFixtureDef4);
			cross1->rigidbody->SetUserData(cross1);

		}

		if (cross2->init()) {
			cross2->set_position(vec2{ -400, -1880 });
			cross2->set_vertex_position2();
			m_cross.emplace_back(cross2);

			cross2->setDef();
			cross2->rigidbody = b2world->CreateBody(&cross2->crossBodyDef);
			cross2->rigidbody->CreateFixture(&cross2->crossFixtureDef);
		//	cross2->rigidbody->CreateFixture(&cross2->crossFixtureDef1);
		//	cross2->rigidbody->CreateFixture(&cross2->crossFixtureDef2);
		//	cross2->rigidbody->CreateFixture(&cross2->crossFixtureDef3);
		//	cross2->rigidbody->CreateFixture(&cross2->crossFixtureDef4);
			cross2->rigidbody->SetUserData(cross2);

			//b2Vec2 position = b2Vec2(-400 * pixeltob2, -1880 * pixeltob2);
			//cross2->rigidbody->SetTransform(position,0);
		}

		/*
		vec2 min = { cross1->vertices[0].x,cross1->vertices[2].y };
		vec2 max = { cross1->vertices[6].x,cross1->vertices[8].y };
		AABB* cross_ab = coll.Abinit(cross1->vertices[2], cross1->vertices[0], cross_a);
		coll.addverindices(cross_ab, cross1->indicess);
		coll.addvertex(cross_ab, cross1->vertices);
		cross_ab->cross = cross1;
		aabbs.emplace_back(cross_ab);*/

		/*
		min = { cross2->vertices[0].x,cross2->vertices[2].y };
		max = { cross2->vertices[6].x,cross2->vertices[8].y };
		AABB* cross_ab2 = coll.Abinit(cross2->vertices[2], cross2->vertices[0], cross_a);
		coll.addverindices(cross_ab2, cross2->indicess);
		coll.addvertex(cross_ab2, cross2->vertices);
		cross_ab2->cross = cross2;
		aabbs.emplace_back(cross_ab2);
		*/
	}
	if(game_level == 1){}
	
	return true;
}


bool World::spawn_fires() {
	if(game_level == 0)
	{   
		Fire fire1;
		Fire fire2;
		Fire fire3;
		Fire fire4;
		Fire fire5;
		Fire fire6;
		/*Fire fire7;
		Fire fire8;*/
		Fire fire9;
		Fire fire10;
		Fire fire11;
		Fire fire12;
		
		fire1.init({ -3600.f, m_player.get_position().y }, { 1, 0 });
		m_fire.emplace_back(fire1);

		fire2.init({ -2900.f, m_player.get_position().y }, { 1, 0 });
		m_fire.emplace_back(fire2);

		fire3.init({ -2000.f, m_player.get_position().y }, { 1, 0 });
		m_fire.emplace_back(fire3);

		fire4.init({ -400.f, m_player.get_position().y }, { 1, 0 });
		m_fire.emplace_back(fire4);

		fire5.init({ -300.f, m_player.get_position().y }, { 1, 0 });
		m_fire.emplace_back(fire5);

		fire6.init({ -200.f, m_player.get_position().y }, { 1, 0 });
		m_fire.emplace_back(fire6);

		/*fire7.init({ 600.f, m_player.get_position().y }, { 1, 0 });
		m_fire.emplace_back(fire7);

		fire8.init({ 1000.f, m_player.get_position().y }, { 1, 0 });
		m_fire.emplace_back(fire8);*/

		fire9.init({ 1600.f, m_player.get_position().y }, { 1, 0 });
		m_fire.emplace_back(fire9);

		fire10.init({ 2300.f, m_player.get_position().y }, { 1, 0 });
		m_fire.emplace_back(fire10);

		fire11.init({ 3100.f, m_player.get_position().y }, { 1, 0 });
		m_fire.emplace_back(fire11);

		fire12.init({ 3600.f, m_player.get_position().y }, { 1, 0 });
		m_fire.emplace_back(fire12);


		for (int i = 0; i < m_fire.size(); i++) {
			/*
			AABB* fire_ab = coll.Abinit(m_fire[i].vertex[2], m_fire[i].vertex[0], fire_a);
			coll.addverindices(fire_ab, m_fire[i].indicess);
			coll.addvertex(fire_ab, m_fire[i].vertex);
			fire_ab->fire = &m_fire[i];
			aabbs.emplace_back(fire_ab);*/

			m_fire[i].setDef();
			m_fire[i].rigidbody = b2world->CreateBody(&m_fire[i].fireBodyDef);
			m_fire[i].rigidbody->CreateFixture(&m_fire[i].fireFixtureDef);
			m_fire[i].rigidbody->SetUserData(&m_fire[i]);
		}
	}

	if (game_level == 2) {
		Fire fire1;
		Fire fire2;
		Fire fire3;
		Fire fire4;
		Fire fire5;
		Fire fire6;

		fire1.init({ 4800.f,270.f }, { 1,0 });
		m_fire.emplace_back(fire1);

		fire2.init({ 4900.f,270.f }, { 1,0 });
		m_fire.emplace_back(fire2);

		fire3.init({ 5000.f,270.f }, { 1,0 });
		m_fire.emplace_back(fire3);

		fire4.init({ 5100.f,270.f }, { 1,0 });
		m_fire.emplace_back(fire4);

		fire5.init({ 5200.f,270.f }, { 1,0 });
		m_fire.emplace_back(fire5);

		fire6.init({ -3500.f,-100.f }, { 1,0 });
		m_fire.emplace_back(fire6);

		for (int i = 0; i < m_fire.size(); i++) {
			m_fire[i].setDef();
			m_fire[i].rigidbody = b2world->CreateBody(&m_fire[i].fireBodyDef);
			m_fire[i].rigidbody->CreateFixture(&m_fire[i].fireFixtureDef);
			m_fire[i].rigidbody->SetUserData(&m_fire[i]);
		}
	}

	if(game_level == 1){
		Fire fire1;

		fire1.init({ -2600.f, 920.f }, { 1, 0 });
		m_fire.emplace_back(fire1);



		for (int i = 0; i < m_fire.size(); i++) {
			/*
			AABB* fire_ab = coll.Abinit(m_fire[i].vertex[2], m_fire[i].vertex[0], fire_a);
			coll.addverindices(fire_ab, m_fire[i].indicess);
			coll.addvertex(fire_ab, m_fire[i].vertex);
			fire_ab->fire = &m_fire[i];
			aabbs.emplace_back(fire_ab);*/

			m_fire[i].setDef();
			m_fire[i].rigidbody = b2world->CreateBody(&m_fire[i].fireBodyDef);
			m_fire[i].rigidbody->CreateFixture(&m_fire[i].fireFixtureDef);
			m_fire[i].rigidbody->SetUserData(&m_fire[i]);
		}
	}

	return true;
}

bool World::spawn_mace() {
	if(game_level == 0){
	}
	if (game_level == 1) {
		Mace * mace1 = new Mace;
		//Mace * mace2 = new Mace;
		mace1->init({ -900.f,-450 });
		//mace2->init({ -1600.f,-700 });
		m_mace.emplace_back(mace1);
		//m_mace.emplace_back(mace2);

		mace1->setDef();
		mace1->rigidbody = b2world->CreateBody(&mace1->maceBodyDef);
		mace1->rigidbody->CreateFixture(&(mace1->maceFixtureDef));
		mace1->rigidbody->SetUserData(mace1);

		for (int i = 0; i < m_mace.size(); i++) {
			/*
			AABB* mace_ab = coll.Abinit(m_mace[i]->vertex[1], m_mace[i]->vertex[0], mace_a);
			coll.addverindices(mace_ab, m_mace[i]->indicess);
			coll.addvertex(mace_ab, m_mace[i]->vertex);
			mace_ab->mace = m_mace[i];
			aabbs.emplace_back(mace_ab);
			*/
			/*
			m_mace[i]->setDef();
			m_mace[i]->rigidbody = b2world->CreateBody(&(m_mace[i]->maceBodyDef));
			m_mace[i]->rigidbody->CreateFixture(&(m_mace[i]->maceFixtureDef));
			m_mace[i]->rigidbody->SetUserData(m_mace[i]);*/

		}
	}
	
	return true;
}

bool World::spawn_swamp() {
	if (game_level == 0) {
	}
	if (game_level == 1) {
	}
	if (game_level == 2) {
		Swamp* swamp0 = new Swamp;
		swamp0->init({ 1300,400 });
		m_swamp.emplace_back(swamp0);

		swamp0->setDef();
		swamp0->rigidbody = b2world->CreateBody(&swamp0->swampBodyDef);
		swamp0->rigidbody->CreateFixture(&swamp0->swampFixtureDef);
		swamp0->rigidbody->SetUserData(swamp0);

		Swamp* swamp1 = new Swamp;
		swamp1->init({ -5000,-50 });
		m_swamp.emplace_back(swamp1);

		swamp1->setDef();
		swamp1->rigidbody = b2world->CreateBody(&swamp1->swampBodyDef);
		swamp1->rigidbody->CreateFixture(&swamp1->swampFixtureDef);
		swamp1->rigidbody->SetUserData(swamp1);

		/*
		b2BodyDef testBodyDef;
		b2PolygonShape testShape;
		b2FixtureDef testFixtureDef;

		testBodyDef.type = b2_dynamicBody;
		testBodyDef.position.Set(-50.f, 5.f);
		testBodyDef.angle = 0;
		//setting box2d shape and fixture def
		//float32 width = abs(m_scale.x* player_texture.width * pixeltob2 * 0.005);
		//float32 height = abs(m_scale.y* player_texture.height * pixeltob2 * 0.005);
		testShape.SetAsBox(1, 1);
		testFixtureDef.shape = &testShape;
		testFixtureDef.density = 1;
		testFixtureDef.friction = 0.3;



		testBall = b2world->CreateBody(&testBodyDef);
		testBall->CreateFixture(&testFixtureDef);*/
		


	}
	return true;
}

bool World::spawn_pillar() {
	if (game_level == 2) {
		Pillar* pillar1 = new Pillar;
		pillar1->init({ 2000,80 });
		m_pillar.emplace_back(pillar1);

		Pillar* pillar2 = new Pillar;
		pillar2->init({ 3400,740 });
		m_pillar.emplace_back(pillar2);

		pillar1->setDef();
		pillar1->rigidbody = b2world->CreateBody(&pillar1->pillarBodyDef);
		pillar1->rigidbody->CreateFixture(&pillar1->pillarFixtureDef);
		pillar1->rigidbody->SetUserData(pillar1);

		pillar2->setDef();
		pillar2->rigidbody = b2world->CreateBody(&pillar2->pillarBodyDef);
		pillar2->rigidbody->CreateFixture(&pillar2->pillarFixtureDef);
		pillar2->rigidbody->SetUserData(pillar2);

	}
	return true;
}

bool World::spawn_chest() {
	if (game_level == 2) {
		Chest* chest1 = new Chest;
		chest1->init({ -4750,1100 });
		m_chest.emplace_back(chest1);

		chest1->setDef();
		chest1->rigidbody = b2world->CreateBody(&chest1->chestBodyDef);
		chest1->rigidbody->CreateFixture(&chest1->chestFixtureDef);
		chest1->rigidbody->SetUserData(chest1);
	}
	return true;
}

bool World::spawn_venom(){
	if (game_level == 2) {
		Venom* v1 = new Venom;
		Venom* v2 = new Venom;
		Venom* v3 = new Venom;
		Venom* v4 = new Venom;
		Venom* v5 = new Venom;
		Venom* v6 = new Venom;
		Venom* v7 = new Venom;
		Venom* v8 = new Venom;
		Venom* v9 = new Venom;
		Venom* v10 = new Venom;
		
		

		v1->init({ -4230.f,-100.f });
		v2->init({ -4240.f,-100.f });
		v3->init({ -4250.f,-100.f });
		v4->init({ -4550.f,-100.f });
		v5->init({ -4800.f,-100.f });
		v6->init({ -5100.f,-100.f });
		v7->init({ -5200.f,-100.f });
		v8->init({ -5500.f,-100.f });
		v9->init({ -3500.f,750.f });
		v10->init({ -3810.f,750.f });
		
		

		m_venom.emplace_back(v1);
		m_venom.emplace_back(v2);
		m_venom.emplace_back(v3);
		m_venom.emplace_back(v4);
		m_venom.emplace_back(v5);
		m_venom.emplace_back(v6);
		m_venom.emplace_back(v7);
		m_venom.emplace_back(v8);
		m_venom.emplace_back(v9);
		m_venom.emplace_back(v10);
		
		

		for (int i = 0; i < m_venom.size(); i++) {		
			m_venom[i]->setDef();
			m_venom[i]->rigidbody = b2world->CreateBody(&(m_venom[i]->venomBodyDef));
			m_venom[i]->rigidbody->CreateFixture(&(m_venom[i]->venomFixtureDef));
			m_venom[i]->rigidbody->SetUserData(m_venom[i]);
		}
	}

	if (game_level == 1) {
		Venom* v1 = new Venom;
		Venom* v2 = new Venom;
		Venom* v3 = new Venom;
		Venom* v4 = new Venom;
		Venom* v5 = new Venom;
		Venom* v6 = new Venom;
		Venom* v7 = new Venom;
		Venom* v8 = new Venom;
		

		v1->init({ 3440.f,-100.f });
		v2->init({ 3510.f,-100.f });
		v3->init({ 3570.f,-100.f });
		v4->init({ 3950.f,-100.f });
		v5->init({ 4200.f,-100.f });
		/*v9->init({ 3400.f,750.f });
		v10->init({ 3550.f,750.f });
		v11->init({ 3700.f,750.f });
		v12->init({ 3850.f,750.f });*/
		v6->init({ 4200.f,1050.f });
		v7->init({ 4450.f,1050.f });
		v8->init({ 4700.f,1050.f });
		

		m_venom.emplace_back(v1);
		m_venom.emplace_back(v2);
		m_venom.emplace_back(v3);
		m_venom.emplace_back(v4);
		m_venom.emplace_back(v5);
		m_venom.emplace_back(v6);
		m_venom.emplace_back(v7);
		m_venom.emplace_back(v8);
		

		for (int i = 0; i < m_venom.size(); i++) {			/*
			AABB* venom_ab = coll.Abinit(m_venom[i]->vertex[1], m_venom[i]->vertex[0], venom_a);
			coll.addverindices(venom_ab, m_venom[i]->indicess);
			coll.addvertex(venom_ab, m_venom[i]->vertex);
			venom_ab->venom = m_venom[i];
			aabbs.emplace_back(venom_ab);
			*/

			m_venom[i]->setDef();
			m_venom[i]->rigidbody = b2world->CreateBody(&(m_venom[i]->venomBodyDef));
			m_venom[i]->rigidbody->CreateFixture(&(m_venom[i]->venomFixtureDef));
			m_venom[i]->rigidbody->SetUserData(m_venom[i]);
		}
	}
	return true;
}

bool World::spawn_grounds() {

	if (game_level == 0) {

		Ground* g1 = new Ground;
		Ground* g0 = new Ground;
		Ground* g2 = new Ground;
		Ground* g3 = new Ground;
		Ground* g4 = new Ground;
		Ground* g5 = new Ground;
		Ground* g6 = new Ground;
		Ground* g7 = new Ground;
		Ground* g8 = new Ground;
		Ground* g9 = new Ground;
		Ground* g10 = new Ground;
		Ground* g11 = new Ground;
		Ground* g12 = new Ground;
		Ground* g13 = new Ground;
		Ground* g14 = new Ground;
		Ground* g15 = new Ground;
		Ground* g16 = new Ground;
		Ground* g17 = new Ground;
		Ground* g18 = new Ground;
		Ground* g19 = new Ground;
		Ground* g20 = new Ground;
		Ground* g21 = new Ground;

		g1->init({ -3700.f, 1400.f });
		grounds.emplace_back(g1);


		/*
		g1->setDef();
	    g1->rigidbody = b2world->CreateBody(&g1->groundBodyDef);
		g1->rigidbody->CreateFixture(&g1->groundFixtureDef);
		g1->rigidbody->SetUserData(g1);
		*/

		g0->init({ -3600.f, 1400.f });
		grounds.emplace_back(g0);

		g2->init({ -3300.f, 1100.f });
		grounds.emplace_back(g2);

		g3->init({ 600.f, 1400.f });
		grounds.emplace_back(g3);

		g4->init({ 700.f, 1400.f });
		grounds.emplace_back(g4);

		g5->init({ 800.f, 1400.f });
		grounds.emplace_back(g5);

		g6->init({ 900.f, 1400.f });
		grounds.emplace_back(g6);

		g7->init({ 1000.f, 1400.f });
		grounds.emplace_back(g7);

		g8->init({ 800.f, 900.f });
		grounds.emplace_back(g8);

		g9->init({ 700.f, 900.f });
		grounds.emplace_back(g9);

		g10->init({ 800.f, 300.f });
		grounds.emplace_back(g10);

		g11->init({ 700.f, 300.f });
		grounds.emplace_back(g11);

		g12->init({ 1100.f, -1200.f });
		grounds.emplace_back(g12);

		g13->init({ 400.f, -1800.f });
		grounds.emplace_back(g13);

		g14->init({ 300.f, -1800.f });
		grounds.emplace_back(g14);

		g15->init({ 200.f, -1800.f });
		grounds.emplace_back(g15);

		g16->init({ 100.f, -1800.f });
		grounds.emplace_back(g16);

		g17->init({ 0.f, -1800.f });
		grounds.emplace_back(g17);

		g18->init({ -100.f, -1800.f });
		grounds.emplace_back(g18);

		g19->init({ -200.f, -1800.f });
		grounds.emplace_back(g19);

		g20->init({ -300.f, -1800.f });
		grounds.emplace_back(g20);

		g21->init({ -400.f, -1800.f });
		grounds.emplace_back(g21);

		for (int i = 0; i < grounds.size(); i++) {
		   
			
			
			AABB* ground_ab = coll.Abinit(grounds[i]->vertex[1], grounds[i]->vertex[0], ground_a);
			coll.addverindices(ground_ab, grounds[i]->indicess);
			coll.addvertex(ground_ab, grounds[i]->vertex);
			ground_ab->ground = grounds[i];
			aabbs.emplace_back(ground_ab);
			

			/*
			grounds[i]->setDef();
			grounds[i]->rigidbody = b2world->CreateBody(&grounds[i]->groundBodyDef);
			grounds[i]->rigidbody->CreateFixture(&grounds[i]->groundFixtureDef);
			grounds[i]->rigidbody->SetUserData(grounds[i]);
			*/
			
		}
	}
	if(game_level == 1){
		Ground* g0 = new Ground;
		Ground* g1 = new Ground;
		Ground* g2 = new Ground;
		Ground* g3 = new Ground;
		Ground* g4 = new Ground;
		Ground* g5 = new Ground;
		Ground* g6 = new Ground;
		Ground* g7 = new Ground;
		Ground* g8 = new Ground;
		Ground* g9 = new Ground;
		Ground* g10 = new Ground;
		Ground* g11 = new Ground;
		Ground* g12 = new Ground;
		Ground* g13 = new Ground;
		Ground* g14 = new Ground;
		Ground* g15 = new Ground;
		Ground* g16 = new Ground;
		Ground* g17 = new Ground;
		Ground* g18 = new Ground;
		Ground* g19 = new Ground;
		Ground* g20 = new Ground;
		Ground* g21 = new Ground;
		Ground* g22 = new Ground;
		Ground* g23 = new Ground;
		Ground* g24 = new Ground;
		Ground* g25 = new Ground;
		Ground* g26 = new Ground;
		Ground* g27 = new Ground;
		Ground* g28 = new Ground;
		Ground* g29 = new Ground;
		Ground* g30 = new Ground;
		Ground* g31 = new Ground;
		Ground* g32 = new Ground;
		Ground* g33 = new Ground;
		Ground* g34 = new Ground;
		Ground* g35 = new Ground;
		Ground* g36 = new Ground;
		Ground* g37 = new Ground;
		Ground* g38 = new Ground;
		Ground* g39 = new Ground;
		Ground* g40 = new Ground;
		Ground* g41 = new Ground;
		Ground* g42 = new Ground;
		Ground* g43 = new Ground;
		Ground* g44 = new Ground;
		Ground* g45 = new Ground;
		Ground* g46 = new Ground;
		Ground* g47 = new Ground;
		Ground* g48 = new Ground;
		Ground* g49 = new Ground;
		Ground* g50 = new Ground;
		Ground* g51 = new Ground;
		Ground* g52 = new Ground;
		Ground* g53 = new Ground;
		Ground* g54 = new Ground;
		Ground* g55 = new Ground;
		Ground* g56 = new Ground;
		Ground* g57 = new Ground;
		Ground* g58 = new Ground;
		Ground* g59 = new Ground;
		Ground* g60 = new Ground;
		Ground* g61 = new Ground;
		Ground* g62 = new Ground;
		Ground* g63 = new Ground;
		Ground* g64 = new Ground;
		Ground* g65 = new Ground;
		Ground* g66 = new Ground;
		Ground* g67 = new Ground;
		Ground* g68 = new Ground;
		Ground* g69 = new Ground;
		Ground* g70 = new Ground;
		Ground* g71 = new Ground;
		Ground* g72 = new Ground;
		Ground* g73 = new Ground;
		Ground* g74 = new Ground;
		Ground* g75 = new Ground;
		Ground* g76 = new Ground;
		Ground* g77 = new Ground;
		Ground* g78 = new Ground;
		Ground* g79 = new Ground;
		Ground* g80 = new Ground;
		Ground* g81 = new Ground;
		Ground* g82 = new Ground;
		Ground* g83 = new Ground;
		Ground* g84 = new Ground;
		Ground* g85 = new Ground;
		Ground* g86 = new Ground;
		Ground* g87 = new Ground;
		Ground* g88 = new Ground;
		Ground* g89 = new Ground;
		Ground* g90 = new Ground;
		Ground* g91 = new Ground;
		Ground* g92 = new Ground;
		Ground* g93 = new Ground;
		Ground* g94 = new Ground;
		Ground* g95 = new Ground;
		Ground* g96 = new Ground;
		Ground* g97 = new Ground;
		Ground* g98 = new Ground;
		Ground* g99 = new Ground;
		Ground* g100 = new Ground;
		Ground* g101 = new Ground;
		Ground* g102 = new Ground;
		Ground* g103 = new Ground;
		Ground* g104 = new Ground;
		Ground* g105 = new Ground;
		Ground* g106 = new Ground;
		Ground* g107 = new Ground;
		Ground* g108= new Ground;
		Ground* g109 = new Ground;
		Ground* g110 = new Ground;
		Ground* g111 = new Ground;
		Ground* g112 = new Ground;
		Ground* g113 = new Ground;
		Ground* g114 = new Ground;
		Ground* g115 = new Ground;
		Ground* g116 = new Ground;
		Ground* g117 = new Ground;
		Ground* g118 = new Ground;

		g0->init({ -6028,380 });
		g1->init({ -5928,380 });
		g2->init({ -5828,380 });
		g3->init({ -5728,380 });
		g4->init({ -5628,380 });
		g5->init({ -5528,380 });
		g6->init({ -5428,380 });

		g7->init({ -5329,415 });
		g8->init({ -5229,415 });
		g9->init({ -5129,415 });
		g10->init({ -5029,415 });
		g11->init({ -4929,415 });
		g12->init({ -4829,415 });
		g13->init({ -4729,415 });

		g14->init({ -4629,447 });
		g15->init({ -4529,447 });
		g16->init({ -4429,447 });

		g17->init({ -4329,480 });
		g18->init({ -4229,480 });
		g19->init({ -4129,480 });

		g20->init({ -4029,514 });
		g21->init({ -3929,514 });
		g22->init({ -3829,514 });
		g23->init({ -3530,1070 });
		g24->init({ -3430,1070 });
		g25->init({ -3330,1070 });
		g26->init({ -3230,1070 });
		g27->init({ -3130,1070 });
		g28->init({ -3030,1070 });
		g29->init({ -2930,1070 });
		g30->init({ -2830,1070 });
		g31->init({ -2730,1070 });
		g32->init({ -2630,1070 });
		g33->init({ -2530,1070 });
		g34->init({ -2430,1070 });
		g35->init({ -2330,1070 });
		g36->init({ -2080,450 });
		g37->init({ -1980,450 });
		g38->init({ -1880,450 });
		g39->init({ -1780,450 });
		g40->init({ -1680,450 });
		g41->init({ -1580,450 });
		g42->init({ -1480,450 });
		g43->init({ -1380,450 });
		g44->init({ -1280,450 });
		g45->init({ -1180,450 });
		g46->init({ -1080,450 });
		g47->init({ -980,450 });
		g48->init({ -880,450 });
		g49->init({ -780,450 });
		g50->init({ -680,480 });
		g51->init({ -580,480 });
		g52->init({ -480,490 });
		g53->init({ -380,490 });
		g54->init({ -280,490 });
		g55->init({ -180,490 });
		g56->init({ 1420,400 });
		g57->init({ 1520,400 });
		g58->init({ 1620,400 });
		g59->init({ 1720,400 });
		g60->init({ 1820,360 });
		g61->init({ 1920,360 });
		g62->init({ 2020,360 });
		g63->init({ 2120,390 });
		g64->init({ 2220,390 });
		g65->init({ 2320,390 });
		g66->init({ 2420,390 });
		g67->init({ 2520,390 });
		g68->init({ 2620,390 });
		g69->init({ 2720,390 });
		g70->init({ 2820,390 });
		g71->init({ 3050,0 });
		g72->init({ 3150,0 });
		g73->init({ 3250,0 });
		g74->init({ 3350,0 });
		g75->init({ 3450,0 });
		g76->init({ 3550,0 });
		g77->init({ 3650,0 });
		g78->init({ 3750,0 });
		g79->init({ 3850,0 });
		g80->init({ 3950,0 });
		g81->init({ 4050,0 });
		g82->init({ 4150,0 });
		g83->init({ 4250,0 });
		g84->init({ 4350,0 });
		g85->init({ 4450,0 });
		g86->init({ 4550,0 });
		g87->init({ 4650,0 });
		g88->init({ 4750,0 });
		g89->init({ 4850,0 });
		g90->init({ 4950,0 });
		g91->init({ 5050,0 });
		g92->init({ 5150,0 });
		g93->init({ 5250,0 });
		g94->init({ 5350,0 });
		g95->init({ 5450,0 });
		g96->init({ 5550,0 });
		g97->init({ 5650,0 });
		g98->init({ 5750,0 });
		g99->init({ 5850,0 });
		g100->init({ 5950,0 });
		g101->init({ 3160,860 });
		g102->init({ 3260,860 });
		g103->init({ 3360,860 });
		g104->init({ 3460,860 });
		g105->init({ 3560,860 });
		g106->init({ 3660,860 });
		g107->init({ 3760,860 });
		g108->init({ 3860,860 });
		g109->init({ 4150,1210 });
		g110->init({ 4250,1210 });
		g111->init({ 4350,1210 });
		g112->init({ 4450,1210 });
		g113->init({ 4050,1210 });
		g114->init({ 4550,1210 });
		g115->init({ 4650,1210 });
		g116->init({ 4750,1210 });
		g117->init({ 4850,1210 });
		g118->init({ 4950,1210 });
		

		grounds.emplace_back(g0);
		grounds.emplace_back(g1);
		grounds.emplace_back(g2);
		grounds.emplace_back(g3);
		grounds.emplace_back(g4);
		grounds.emplace_back(g5);
		grounds.emplace_back(g6);
		grounds.emplace_back(g7);
		grounds.emplace_back(g8);
		grounds.emplace_back(g9);
		grounds.emplace_back(g10);
		grounds.emplace_back(g11);
		grounds.emplace_back(g12);
		grounds.emplace_back(g13);
		grounds.emplace_back(g14);
		grounds.emplace_back(g15);
		grounds.emplace_back(g16);
		grounds.emplace_back(g17);
		grounds.emplace_back(g18);
		grounds.emplace_back(g19);
		grounds.emplace_back(g20);
		grounds.emplace_back(g21);
		grounds.emplace_back(g22);
		grounds.emplace_back(g23);
		grounds.emplace_back(g24);
		grounds.emplace_back(g25);
		grounds.emplace_back(g26);
		grounds.emplace_back(g27);
		grounds.emplace_back(g28);
		grounds.emplace_back(g29);
		grounds.emplace_back(g30);
		grounds.emplace_back(g31);
		grounds.emplace_back(g32);
		grounds.emplace_back(g33);
		grounds.emplace_back(g34);
		grounds.emplace_back(g35);
		grounds.emplace_back(g36);
		grounds.emplace_back(g37);
		grounds.emplace_back(g38);
		grounds.emplace_back(g39);
		grounds.emplace_back(g40);
		grounds.emplace_back(g41);
		grounds.emplace_back(g42);
		grounds.emplace_back(g43);
		grounds.emplace_back(g44);
		grounds.emplace_back(g45);
		grounds.emplace_back(g46);
		grounds.emplace_back(g47);
		grounds.emplace_back(g48);
		grounds.emplace_back(g49);
		grounds.emplace_back(g50);
		grounds.emplace_back(g51);
		grounds.emplace_back(g52);
		grounds.emplace_back(g53);
		grounds.emplace_back(g54);
		grounds.emplace_back(g55);
		grounds.emplace_back(g56);
		grounds.emplace_back(g57);
		grounds.emplace_back(g58);
		grounds.emplace_back(g59);
		grounds.emplace_back(g60);
		grounds.emplace_back(g61);
		grounds.emplace_back(g62);
		grounds.emplace_back(g63);
		grounds.emplace_back(g64);
		grounds.emplace_back(g65);
		grounds.emplace_back(g66);
		grounds.emplace_back(g67);
		grounds.emplace_back(g68);
		grounds.emplace_back(g69);
		grounds.emplace_back(g70);
		grounds.emplace_back(g71);
		grounds.emplace_back(g72);
		grounds.emplace_back(g73);
		grounds.emplace_back(g74);
		grounds.emplace_back(g75);
		grounds.emplace_back(g76);
		grounds.emplace_back(g77);
		grounds.emplace_back(g78);
		grounds.emplace_back(g79);
		grounds.emplace_back(g80);
		grounds.emplace_back(g81);
		grounds.emplace_back(g82);
		grounds.emplace_back(g83);
		grounds.emplace_back(g84);
		grounds.emplace_back(g85);
		grounds.emplace_back(g86);
		grounds.emplace_back(g87);
		grounds.emplace_back(g88);
		grounds.emplace_back(g89);
		grounds.emplace_back(g90);
		grounds.emplace_back(g91);
		grounds.emplace_back(g92);
		grounds.emplace_back(g93);
		grounds.emplace_back(g94);
		grounds.emplace_back(g95);
		grounds.emplace_back(g96);
		grounds.emplace_back(g97);
		grounds.emplace_back(g98);
		grounds.emplace_back(g99);
		grounds.emplace_back(g100);
		grounds.emplace_back(g101);
		grounds.emplace_back(g102);
		grounds.emplace_back(g103);
		grounds.emplace_back(g104);
		grounds.emplace_back(g105);
		grounds.emplace_back(g106);
		grounds.emplace_back(g107);
		grounds.emplace_back(g108);
		grounds.emplace_back(g109);
		grounds.emplace_back(g110);
		grounds.emplace_back(g111);
		grounds.emplace_back(g112);
		grounds.emplace_back(g113);
		grounds.emplace_back(g114);
		grounds.emplace_back(g115);
		grounds.emplace_back(g116);
		grounds.emplace_back(g117);
		grounds.emplace_back(g118);

		for (int i = 0; i < grounds.size(); i++) {
			grounds[i]->visible = false;
		}

		for (int i = 0; i < grounds.size(); i++) {
			
			
			AABB* ground_ab = coll.Abinit(grounds[i]->vertex[1], grounds[i]->vertex[0], ground_a);
			coll.addverindices(ground_ab, grounds[i]->indicess);
			coll.addvertex(ground_ab, grounds[i]->vertex);
			ground_ab->ground = grounds[i];
			aabbs.emplace_back(ground_ab);
			
			/*
			grounds[i]->setDef();
			grounds[i]->rigidbody = b2world->CreateBody(&grounds[i]->groundBodyDef);
			grounds[i]->rigidbody->CreateFixture(&grounds[i]->groundFixtureDef);
			grounds[i]->rigidbody->SetUserData(grounds[i]);
			*/
			

		}
	}

	if (game_level == 2) {
		Ground* g0 = new Ground;
		Ground* g1 = new Ground;
		Ground* g2 = new Ground;
		Ground* g3 = new Ground;
		Ground* g4 = new Ground;
		Ground* g5 = new Ground;
		Ground* g6 = new Ground;
		Ground* g7 = new Ground;
		Ground* g8 = new Ground;
		Ground* g9 = new Ground;
		Ground* g10 = new Ground;
		Ground* g11 = new Ground;
		Ground* g12 = new Ground;
		Ground* g13 = new Ground;
		Ground* g14 = new Ground;
		Ground* g15 = new Ground;
		Ground* g16 = new Ground;
		Ground* g17 = new Ground;
		Ground* g18 = new Ground;
		Ground* g19 = new Ground;
		Ground* g20 = new Ground;
		Ground* g21 = new Ground;
		Ground* g22 = new Ground;
		Ground* g23 = new Ground;
		Ground* g24 = new Ground;
		Ground* g25 = new Ground;
		Ground* g26 = new Ground;
		Ground* g27 = new Ground;
		Ground* g28 = new Ground;
		Ground* g29 = new Ground;
		Ground* g30 = new Ground;
		Ground* g31 = new Ground;
		Ground* g32 = new Ground;
		Ground* g33 = new Ground;
		Ground* g34 = new Ground;
		Ground* g35 = new Ground;
		Ground* g36 = new Ground;
		Ground* g37 = new Ground;
		Ground* g38 = new Ground;
		Ground* g39 = new Ground;
		Ground* g40 = new Ground;
		Ground* g41 = new Ground;
		Ground* g42 = new Ground;
		Ground* g43 = new Ground;
		Ground* g44 = new Ground;
		Ground* g45 = new Ground;
		Ground* g46 = new Ground;
		Ground* g47 = new Ground;
		Ground* g48 = new Ground;
		Ground* g49 = new Ground;
		Ground* g50 = new Ground;
		Ground* g51 = new Ground;
		Ground* g52 = new Ground;
		Ground* g53 = new Ground;
		Ground* g54 = new Ground;
		Ground* g55 = new Ground;
		Ground* g56 = new Ground;
		Ground* g57 = new Ground;
		Ground* g58 = new Ground;
		Ground* g59 = new Ground;
		Ground* g60 = new Ground;
		Ground* g61 = new Ground;
		Ground* g62 = new Ground;
		Ground* g63 = new Ground;
		Ground* g64 = new Ground;
		Ground* g65 = new Ground;
		Ground* g66 = new Ground;
		Ground* g67 = new Ground;
		Ground* g68 = new Ground;
		Ground* g69 = new Ground;
		Ground* g70 = new Ground;
		Ground* g71 = new Ground;
		Ground* g72 = new Ground;
		Ground* g73 = new Ground;
		Ground* g74 = new Ground;
		Ground* g75 = new Ground;
		Ground* g76 = new Ground;
		Ground* g77 = new Ground;
		Ground* g78 = new Ground;
		Ground* g79 = new Ground;
		Ground* g80 = new Ground;
		Ground* g81 = new Ground;
		Ground* g82 = new Ground;
		Ground* g83 = new Ground;
		Ground* g84 = new Ground;
		Ground* g85 = new Ground;
		Ground* g86 = new Ground;
		Ground* g87 = new Ground;
		Ground* g88 = new Ground;
		Ground* g89 = new Ground;
		Ground* g90 = new Ground;
		Ground* g91 = new Ground;
		Ground* g92 = new Ground;
		Ground* g93 = new Ground;
		Ground* g94 = new Ground;
		Ground* g95 = new Ground;
		Ground* g96 = new Ground;
		Ground* g97 = new Ground;
		Ground* g98 = new Ground;
		Ground* g99 = new Ground;
		Ground* g100 = new Ground;
		Ground* g101 = new Ground;
		Ground* g102 = new Ground;
		Ground* g103 = new Ground;
		Ground* g104 = new Ground;
		Ground* g105 = new Ground;
		Ground* g106 = new Ground;
		Ground* g107 = new Ground;
		Ground* g108 = new Ground;
		Ground* g109 = new Ground;
		Ground* g110 = new Ground;
		Ground* g111 = new Ground;
		Ground* g112 = new Ground;
		Ground* g113 = new Ground;
		Ground* g114 = new Ground;
		Ground* g115 = new Ground;
		Ground* g116 = new Ground;
		Ground* g117 = new Ground;
		Ground* g118 = new Ground;
		Ground* g119 = new Ground;
		Ground* g120 = new Ground;
		Ground* g121 = new Ground;
		Ground* g122 = new Ground;
		Ground* g123 = new Ground;
		Ground* g124 = new Ground;
		Ground* g125 = new Ground;
		Ground* g126 = new Ground;
		Ground* g127 = new Ground;
		//Ground* g128 = new Ground;
		//Ground* g129 = new Ground;
		Ground* g130 = new Ground;
		Ground* g131 = new Ground;
		Ground* g132 = new Ground;
		Ground* g133 = new Ground;
		Ground* g134 = new Ground;
		Ground* g135 = new Ground;
		Ground* g136 = new Ground;
		Ground* g137 = new Ground;
		Ground* g138 = new Ground;
		Ground* g139 = new Ground;
		Ground* g140 = new Ground;
		Ground* g141 = new Ground;
		Ground* g142 = new Ground;
		Ground* g143 = new Ground;
		



		g0->init({ -6028,0 });
		g1->init({ -5928,0 });
		g2->init({ -5828,0 });
		g3->init({ -5728,0 });
		g4->init({ -5628,0 });
		g5->init({ -5528,0 });
		g6->init({ -5428,0 });

		g7->init({ -5329,0 });
		g8->init({ -5229,0 });
		g9->init({ -5129,0 });
		g10->init({ -5029,0 });
		g11->init({ -4929,0 });
		g12->init({ -4829,0 });
		g13->init({ -4729,0 });
		g14->init({ -4629,0 });
		g15->init({ -4529,0 });
		g16->init({ -4429,0 });
		g17->init({ -4329,0 });
		g18->init({ -4229,0 });
		g19->init({ -4129,0 });
		g20->init({ -4029,0 });
		g21->init({ -3929,0 });
		g22->init({ -3829,0 });

		g23->init({ -3729,0 });
		g24->init({ -3629,0 });
		g25->init({ -3529,0 });
		g26->init({ -3429,0 });
		g27->init({ -3329,0 });
		g28->init({ -3229,0 });
		g29->init({ -3129,0 });
		g30->init({ -3029,0 });
		g31->init({ -2820,390 });
		g32->init({ -2720,390 });
		g33->init({ -2620,390 });
		g34->init({ -2520,390 });
		g35->init({ -2420,390 });
		g36->init({ -2320,390 });
		g37->init({ -2220,390 });
		g38->init({ -2120,390 });
		g39->init({ -2020,360 });
		g40->init({ -1920,360 });
		g41->init({ -1820,360 });

		g42->init({ -1720,390 });
		g43->init({ -1620,390 });
		g44->init({ -1520,390 });
		g45->init({ -1420,390 });

		g46->init({ -3930,860 });
		g47->init({ -3830,860 });
		g48->init({ -3730,860});
		g49->init({ -3630,860});
		g50->init({ -3530,860});
		g51->init({ -3430,860});
		g52->init({ -3330,860 });
		g53->init({ -3230,860});
		g54->init({ -3130,860 });

		g55->init({ -4960,1210 });
		g56->init({ -4860,1210 });
		g57->init({ -4760,1210 });
		g58->init({ -4660,1210 });
		g59->init({ -4560,1210 });
		g60->init({ -4460,1210 });
		g61->init({ -4360,1210 });
		g62->init({ -4260,1210 });
		g63->init({ -4160,1210 });
		g64->init({ -4060,1210 });
		g65->init({ 2330,1070 });
		g66->init({ 2430,1070 });
		g67->init({ 2530,1070 });
		g68->init({ 2630,1070 });
		g69->init({ 2730,1070 });
		g70->init({ 2830,1070 });
		g71->init({ 2930,1070 });
		g72->init({ 3030,1070 });
		g73->init({ 3130,1070 });
		g74->init({ 3230,1070 });
		g75->init({ 3330,1070 });
		g76->init({ 3430,1070 });
		g77->init({ 3530,1070 });

		g78->init({ 780,450 });
		g79->init({ 880,450 });
		g80->init({ 980,450 });
		g81->init({ 1080,450 });
		g82->init({ 1180,450 });
		g83->init({ 1280,450 });
		g84->init({ 1380,450 });
		g85->init({ 1480,450 });
		g86->init({ 1580,450 });
		g87->init({ 1680,450 });
		g88->init({ 1780,450 });
		g89->init({ 1880,450 });
		g90->init({ 1980,450 });
		g91->init({ 2080,450 });

		g92->init({ 180,490 });
		g93->init({ 280,490 });
		g94->init({ 380,490 });
		g95->init({ 480,490 });
		g96->init({ 580,480 });
		g97->init({ 680,480 });

		g98->init({ 3879,530 });
		g99->init({ 3979,530 });
		g100->init({ 4079,530 });
		g101->init({ 4179,490 });
		g102->init({ 4279,490 });
		g103->init({ 4379,490 });
		g104->init({ 4479,457 });
		g105->init({ 4579,457 });
		g106->init({ 4679,457 });
		g107->init({ 3779,550 });

		g108->init({ 4779,415 });
		g109->init({ 4879,415 });
		g110->init({ 4979,415 });
		g111->init({ 5079,415 });
		g112->init({ 5179,415 });
		g113->init({ 5279,415 });
		g114->init({ 5379,415 });
		g115->init({ 5479,380 });
		g116->init({ 5579,380 });
		g117->init({ 5679,380 });
		g118->init({ 5779,380 });
		g119->init({ 5879,380 });
		g120->init({ 5979,380 });


		g121->init({ -1000,180 });
		g122->init({ -900,180 });
		g123->init({ -800,180 });
		g124->init({ -700,180 });
		g125->init({ -600,180 });
		g126->init({ -500,180 });
		g127->init({ -400,180 });
		//g128->init({ 2300,760 });
		//g129->init({ 3560,760 });
		g130->init({ -900,180 });
		g131->init({ -300,180 });
		g132->init({ -200,180 });
		g133->init({ -1000,610 });
		g134->init({ -900,610 });
		g135->init({ -800,610 });
		g136->init({ -700,610 });
		g137->init({ -600,610 });
		g138->init({ -500,610 });
		g139->init({ -400,610 });
		g140->init({ -300,610 });
		g141->init({ -200,610 });
		g142->init({ -1100,610 });
		g143->init({ -100,180 });
		
		

		grounds.emplace_back(g0);
		grounds.emplace_back(g1);
		grounds.emplace_back(g2);
		grounds.emplace_back(g3);
		grounds.emplace_back(g4);
		grounds.emplace_back(g5);
		grounds.emplace_back(g6);
		grounds.emplace_back(g7);
		grounds.emplace_back(g8);
		grounds.emplace_back(g9);
		grounds.emplace_back(g10);
		grounds.emplace_back(g11);
		grounds.emplace_back(g12);
		grounds.emplace_back(g13);
		grounds.emplace_back(g14);
		grounds.emplace_back(g15);
		grounds.emplace_back(g16);
		grounds.emplace_back(g17);
		grounds.emplace_back(g18);
		grounds.emplace_back(g19);
		grounds.emplace_back(g20);
		grounds.emplace_back(g21);
		grounds.emplace_back(g22);
		grounds.emplace_back(g23);
		grounds.emplace_back(g24);
		grounds.emplace_back(g25);
		grounds.emplace_back(g26);
		grounds.emplace_back(g27);
		grounds.emplace_back(g28);
		grounds.emplace_back(g29);
		grounds.emplace_back(g30);
		grounds.emplace_back(g31);
		grounds.emplace_back(g32);
		grounds.emplace_back(g33);
		grounds.emplace_back(g34);
		grounds.emplace_back(g35);
		grounds.emplace_back(g36);
		grounds.emplace_back(g37);
		grounds.emplace_back(g38);
		grounds.emplace_back(g39);
		grounds.emplace_back(g40);
		grounds.emplace_back(g41);
		grounds.emplace_back(g42);
		grounds.emplace_back(g43);
		grounds.emplace_back(g44);
		grounds.emplace_back(g45);
		grounds.emplace_back(g46);
		grounds.emplace_back(g47);
		grounds.emplace_back(g48);
		grounds.emplace_back(g49);
		grounds.emplace_back(g50);
		grounds.emplace_back(g51);
		grounds.emplace_back(g52);
		grounds.emplace_back(g53);
		grounds.emplace_back(g54);
		grounds.emplace_back(g55);
		grounds.emplace_back(g56);
		grounds.emplace_back(g57);
		grounds.emplace_back(g58);
		grounds.emplace_back(g59);
		grounds.emplace_back(g60);
		grounds.emplace_back(g61);
		grounds.emplace_back(g62);
		grounds.emplace_back(g63);
		grounds.emplace_back(g64);
		grounds.emplace_back(g65);
		grounds.emplace_back(g66);
		grounds.emplace_back(g67);
		grounds.emplace_back(g68);
		grounds.emplace_back(g69);
		grounds.emplace_back(g70);
		grounds.emplace_back(g71);
		grounds.emplace_back(g72);
		grounds.emplace_back(g73);
		grounds.emplace_back(g74);
		grounds.emplace_back(g75);
		grounds.emplace_back(g76);
		grounds.emplace_back(g77);
		grounds.emplace_back(g78);
		grounds.emplace_back(g79);
		grounds.emplace_back(g80);
		grounds.emplace_back(g81);
		grounds.emplace_back(g82);
		grounds.emplace_back(g83);
		grounds.emplace_back(g84);
		grounds.emplace_back(g85);
		grounds.emplace_back(g86);
		grounds.emplace_back(g87);
		grounds.emplace_back(g88);
		grounds.emplace_back(g89);
		grounds.emplace_back(g90);
		grounds.emplace_back(g91);
		grounds.emplace_back(g92);
		grounds.emplace_back(g93);
		grounds.emplace_back(g94);
		grounds.emplace_back(g95);
		grounds.emplace_back(g96);
		grounds.emplace_back(g97);
		grounds.emplace_back(g98);
		grounds.emplace_back(g99);
		grounds.emplace_back(g100);
		grounds.emplace_back(g101);
		grounds.emplace_back(g102);
		grounds.emplace_back(g103);
		grounds.emplace_back(g104);
		grounds.emplace_back(g105);
		grounds.emplace_back(g106);
		grounds.emplace_back(g107);
		grounds.emplace_back(g108);
		grounds.emplace_back(g109);
		grounds.emplace_back(g110);
		grounds.emplace_back(g111);
		grounds.emplace_back(g112);
		grounds.emplace_back(g113);
		grounds.emplace_back(g114);
		grounds.emplace_back(g115);
		grounds.emplace_back(g116);
		grounds.emplace_back(g117);
		grounds.emplace_back(g118);
		grounds.emplace_back(g119);
		grounds.emplace_back(g120);
		grounds.emplace_back(g121);
		grounds.emplace_back(g122);
		grounds.emplace_back(g123);
		grounds.emplace_back(g124);
		grounds.emplace_back(g125);
		grounds.emplace_back(g126);
		grounds.emplace_back(g127);
		//grounds.emplace_back(g128);
		//grounds.emplace_back(g129);
		grounds.emplace_back(g130);
		grounds.emplace_back(g131);
		grounds.emplace_back(g132);
		grounds.emplace_back(g133);
		grounds.emplace_back(g134);
		grounds.emplace_back(g135);
		grounds.emplace_back(g136);
		grounds.emplace_back(g137);
		grounds.emplace_back(g138);
		grounds.emplace_back(g139);
		grounds.emplace_back(g140);
		grounds.emplace_back(g141);
		grounds.emplace_back(g142);
		grounds.emplace_back(g143);


		
		


		for (int i = 0; i < grounds.size() - 21; i++) {
			grounds[i]->visible = false;
		}


		for (int i = 0; i < grounds.size(); i++) {
			
			
			AABB* ground_ab = coll.Abinit(grounds[i]->vertex[1], grounds[i]->vertex[0], ground_a);
			coll.addverindices(ground_ab, grounds[i]->indicess);
			coll.addvertex(ground_ab, grounds[i]->vertex);
			ground_ab->ground = grounds[i];
			aabbs.emplace_back(ground_ab);
			
			/*
			grounds[i]->setDef();
			grounds[i]->rigidbody = b2world->CreateBody(&grounds[i]->groundBodyDef);
			grounds[i]->rigidbody->CreateFixture(&grounds[i]->groundFixtureDef);
			grounds[i]->rigidbody->SetUserData(grounds[i]);
			
			*/

		}

	}
	
	return true;
	
}

bool World::spawn_jumporbs() {
	if(game_level == 0){
		JumpOrb* o0 = new JumpOrb;
		o0->init({ -4000, 1200 });
		jumporbs.emplace_back(o0);

		JumpOrb* o1 = new JumpOrb;
		o1->init({ 1000, 1000 });
		jumporbs.emplace_back(o1);

		JumpOrb* o2 = new JumpOrb;
		o2->init({ 800, 500 });
		jumporbs.emplace_back(o2);

		JumpOrb* o3 = new JumpOrb;
		o3->init({ 1200, 400 });
		jumporbs.emplace_back(o3);

		JumpOrb* o4 = new JumpOrb;
		o4->init({ 700, -200 });
		jumporbs.emplace_back(o4);

		JumpOrb* o5 = new JumpOrb;
		o5->init({ 700, -500 });
		jumporbs.emplace_back(o5);

		JumpOrb* o6 = new JumpOrb;
		o6->init({ 700, -1100 });
		jumporbs.emplace_back(o6);

		JumpOrb* o7 = new JumpOrb;
		o7->init({ 1100, -1600 });
		jumporbs.emplace_back(o7);

		JumpOrb* o8 = new JumpOrb;
		o8->init({ 800, -1800 });
		jumporbs.emplace_back(o8);

		JumpOrb* o9 = new JumpOrb;
		o9->init({ 3700, 1300 });
		jumporbs.emplace_back(o9);

		JumpOrb* o10 = new JumpOrb;
		o10->init({ 3500, 1000 });
		jumporbs.emplace_back(o10);

		JumpOrb* o11 = new JumpOrb;
		o11->init({ 3600, 600 });
		jumporbs.emplace_back(o11);

		for (int i = 0; i < jumporbs.size(); i++) {

			/*
			AABB* jumporb_ab = coll.Abinit(jumporbs[i]->vertex[1], jumporbs[i]->vertex[0], jump_orb_a);
			coll.addverindices(jumporb_ab, jumporbs[i]->indicess);
			coll.addvertex(jumporb_ab, jumporbs[i]->vertex);
			jumporb_ab->orb = jumporbs[i];
			aabbs.emplace_back(jumporb_ab);
			*/
			
			jumporbs[i]->setDef();
			jumporbs[i]->rigidbody = b2world->CreateBody(&(jumporbs[i]->jumpOrbBodyDef));
			jumporbs[i]->rigidbody->CreateFixture(&(jumporbs[i]->jumpOrbFixtureDef));
			jumporbs[i]->rigidbody->SetUserData(jumporbs[i]);
		}
	}
	if(game_level == 1){
		JumpOrb* o0 = new JumpOrb;
		JumpOrb* o1 = new JumpOrb;
		JumpOrb* o2 = new JumpOrb;
		JumpOrb* o3 = new JumpOrb;
		JumpOrb* o4 = new JumpOrb;
		JumpOrb* o5 = new JumpOrb;
		JumpOrb* o6 = new JumpOrb;
		JumpOrb* o7 = new JumpOrb;
		JumpOrb* o8 = new JumpOrb;
		JumpOrb* o9 = new JumpOrb;
		JumpOrb* o10 = new JumpOrb;

		o0->init({ 3000, 500 });
		o1->init({ 2800, 50 });
		o2->init({ -2300, 700 });
		o3->init({ -1600, 0 });
		o4->init({ -1600, -500 });
		o5->init({ 200, 100 });
		o6->init({ 600, 0 });
		o7->init({ 1000, -100 });
		o8->init({ 1300, 100 });
		o9->init({ 5190, 900 });
		o10->init({ 5400, 700 });
		

		jumporbs.emplace_back(o0);
		jumporbs.emplace_back(o1);
		jumporbs.emplace_back(o2);
		jumporbs.emplace_back(o3);
		jumporbs.emplace_back(o4);
		jumporbs.emplace_back(o5);
		jumporbs.emplace_back(o6);
		jumporbs.emplace_back(o7);
		jumporbs.emplace_back(o8);
		jumporbs.emplace_back(o9);
		jumporbs.emplace_back(o10);

		for (int i = 0; i < jumporbs.size(); i++) {
			/*
			AABB* jumporb_ab = coll.Abinit(jumporbs[i]->vertex[1], jumporbs[i]->vertex[0], jump_orb_a);
			coll.addverindices(jumporb_ab, jumporbs[i]->indicess);
			coll.addvertex(jumporb_ab, jumporbs[i]->vertex);
			jumporb_ab->orb = jumporbs[i];
			aabbs.emplace_back(jumporb_ab);*/

			
			jumporbs[i]->setDef();
			jumporbs[i]->rigidbody = b2world->CreateBody(&(jumporbs[i]->jumpOrbBodyDef));
			jumporbs[i]->rigidbody->CreateFixture(&(jumporbs[i]->jumpOrbFixtureDef));
			jumporbs[i]->rigidbody->SetUserData(jumporbs[i]);
		}
	}

	if (game_level == 2) {
		JumpOrb* o0 = new JumpOrb;
		o0->init(vec2{-1150,-200});
		jumporbs.emplace_back(o0);

		JumpOrb* o1 = new JumpOrb;
		o1->init(vec2{ -1000,-600 });
		jumporbs.emplace_back(o1);

		JumpOrb* o2 = new JumpOrb;
		o2->init(vec2{ -50,750 });
		jumporbs.emplace_back(o2);

		JumpOrb* o3 = new JumpOrb;
		o3->init(vec2{ -50,1100 });
		jumporbs.emplace_back(o3);

		JumpOrb* o4 = new JumpOrb;
		o4->init(vec2{ 360,950 });
		jumporbs.emplace_back(o4);

		JumpOrb* o5 = new JumpOrb;
		o5->init(vec2{ 3500,760 });
		jumporbs.emplace_back(o5);

		JumpOrb* o6 = new JumpOrb;
		o6->init(vec2{ 2350,720 });
		jumporbs.emplace_back(o6);

		JumpOrb* o7 = new JumpOrb;
		o7->init(vec2{ -3100,450 });
		jumporbs.emplace_back(o7);

		for (int i = 0; i < jumporbs.size(); i++) {

			/*
			AABB* jumporb_ab = coll.Abinit(jumporbs[i]->vertex[1], jumporbs[i]->vertex[0], jump_orb_a);
			coll.addverindices(jumporb_ab, jumporbs[i]->indicess);
			coll.addvertex(jumporb_ab, jumporbs[i]->vertex);
			jumporb_ab->orb = jumporbs[i];
			aabbs.emplace_back(jumporb_ab);*/

			
			jumporbs[i]->setDef();
			jumporbs[i]->rigidbody = b2world->CreateBody(&(jumporbs[i]->jumpOrbBodyDef));
			jumporbs[i]->rigidbody->CreateFixture(&(jumporbs[i]->jumpOrbFixtureDef));
			jumporbs[i]->rigidbody->SetUserData(jumporbs[i]);
		}
	}


	return true;
}

bool World::spawn_goldenorbs() {
	if(game_level == 0){
		GoldenOrb *o0 = new GoldenOrb;
		o0->init({ -2700, 1600 });
		o0->draw_state = 1;
		o0->rigidbody = b2world->CreateBody(&o0->goldenBodyDef);
		o0->rigidbody->CreateFixture(&o0->goldenFixtureDef);
		o0->rigidbody->SetUserData(o0);
		goldenorbs.emplace_back(o0);

		GoldenOrb *o1 = new GoldenOrb;
		o1->init({ -3700, 800 });
		o1->draw_state = 1;
		o1->rigidbody = b2world->CreateBody(&o1->goldenBodyDef);
		o1->rigidbody->CreateFixture(&o1->goldenFixtureDef);
		o1->rigidbody->SetUserData(o1);
		goldenorbs.emplace_back(o1);

		GoldenOrb *o2 = new GoldenOrb;
		o2->init({ 0, -1900 });
		o2->draw_state = 1;
		o2->rigidbody = b2world->CreateBody(&o2->goldenBodyDef);
		o2->rigidbody->CreateFixture(&o2->goldenFixtureDef);
		o2->rigidbody->SetUserData(o2);
		goldenorbs.emplace_back(o2);

		GoldenOrb *o3 = new GoldenOrb;
		o3->init({ 3400, 200 });
		o3->draw_state = 1;
		o3->rigidbody = b2world->CreateBody(&o3->goldenBodyDef);
		o3->rigidbody->CreateFixture(&o3->goldenFixtureDef);
		o3->rigidbody->SetUserData(o3);
		goldenorbs.emplace_back(o3);

		/*
		for (int i = 0; i < goldenorbs.size(); i++) {
			AABB* gold_ab = coll.Abinit(goldenorbs[i]->vertex[1], goldenorbs[i]->vertex[0], gold_orb_a);
			coll.addverindices(gold_ab, goldenorbs[i]->indicess);
			coll.addvertex(gold_ab, goldenorbs[i]->vertex);
			gold_ab->gold = goldenorbs[i];
			aabbs.emplace_back(gold_ab);
		
		}*/
	}
	if(game_level == 1){
		GoldenOrb *o0 = new GoldenOrb;
		GoldenOrb *o1 = new GoldenOrb;
		GoldenOrb *o2 = new GoldenOrb;
		GoldenOrb *o3 = new GoldenOrb;


		o0->init({ 4400.f, -100.f });
		o1->init({ 5000.f, 500.f });
		o2->init({ -1600.f, -1100.f });
		o3->init({ 2400.f, 150.f });
		

		o0->draw_state = 1;
		o1->draw_state = 1;
		o2->draw_state = 1;
		o3->draw_state = 1;

		o0->rigidbody = b2world->CreateBody(&o0->goldenBodyDef);
		o0->rigidbody->CreateFixture(&o0->goldenFixtureDef);
		o0->rigidbody->SetUserData(o0);

		o1->rigidbody = b2world->CreateBody(&o1->goldenBodyDef);
		o1->rigidbody->CreateFixture(&o1->goldenFixtureDef);
		o1->rigidbody->SetUserData(o1);

		o2->rigidbody = b2world->CreateBody(&o2->goldenBodyDef);
		o2->rigidbody->CreateFixture(&o2->goldenFixtureDef);
		o2->rigidbody->SetUserData(o2);

		o3->rigidbody = b2world->CreateBody(&o3->goldenBodyDef);
		o3->rigidbody->CreateFixture(&o3->goldenFixtureDef);
		o3->rigidbody->SetUserData(o3);

		goldenorbs.emplace_back(o0);
		goldenorbs.emplace_back(o1);
		goldenorbs.emplace_back(o2);
		goldenorbs.emplace_back(o3);

		/*
		for (int i = 0; i < goldenorbs.size(); i++) {
			AABB* gold_ab = coll.Abinit(goldenorbs[i]->vertex[1], goldenorbs[i]->vertex[0], gold_orb_a);
			coll.addverindices(gold_ab, goldenorbs[i]->indicess);
			coll.addvertex(gold_ab, goldenorbs[i]->vertex);
			gold_ab->gold = goldenorbs[i];
			aabbs.emplace_back(gold_ab);
		}*/
	}

	if (game_level == 2) {
		GoldenOrb *o1 = new GoldenOrb;
		o1->init({ -1300.f, -750.f });
		o1->draw_state = 1;
		goldenorbs.emplace_back(o1);

		GoldenOrb *o2 = new GoldenOrb;
		o2->init({ -50.f, 1000.f });
		o2->draw_state = 1;
		goldenorbs.emplace_back(o2);

		GoldenOrb *o3 = new GoldenOrb;
		o3->init({ 5800.f, 300.f });
		o3->draw_state = 1;
		goldenorbs.emplace_back(o3);

		for (int i = 0; i < goldenorbs.size(); i++) {
			goldenorbs[i]->rigidbody = b2world->CreateBody(&goldenorbs[i]->goldenBodyDef);
			goldenorbs[i]->rigidbody->CreateFixture(&goldenorbs[i]->goldenFixtureDef);
			goldenorbs[i]->rigidbody->SetUserData(goldenorbs[i]);
		}
		/*o0->rigidbody = b2world->CreateBody(&o0->goldenBodyDef);
		o0->rigidbody->CreateFixture(&o0->goldenFixtureDef);
		o0->rigidbody->SetUserData(o0);*/

		
	}
	
	return true;
}

bool World::spawn_stones()
{
	if (game_level == 0) {
	}
	if (game_level == 1) {
		Stone * s1 = new Stone;
		s1->init({ -4800.f, -60.f });
		stones.emplace_back(s1);

		/*
		AABB* stone_ab = coll.Abinit(s1->vertex[2], s1->vertex[0], stone_a);
		coll.addverindices(stone_ab, s1->indicess);
		coll.addvertex(stone_ab, s1->vertex);
		stone_ab->sstone = s1;
		aabbs.emplace_back(stone_ab);*/

		s1->setDef();

		s1->rigidbody = b2world->CreateBody(&s1->stoneBodyDef);

		s1->rigidbody->CreateFixture(&s1->stoneFixtureDef);
		//s1->rigidbody->CreateFixture(&s1->stoneSensorFixtureDef);
		s1->rigidbody->SetUserData(s1);

	}
	
	return true;
}


// Creates a new enemy and if successfull adds it to the list of enemys
bool World::spawn_enemy()
{
	//Skull enemy;
	if (game_level == 0) {
		Skull* enemy = new Skull;
		Skull* enemy1 = new Skull;
		Skull* enemy2 = new Skull;
		Skull* enemy3 = new Skull;
		Skull* enemy4 = new Skull;
		Skull* enemy5 = new Skull;
		Skull* enemy6 = new Skull;
		Skull* enemy7 = new Skull;
		Knight* knight = new Knight;
		Knight* knight1 = new Knight;
		Knight* knight2 = new Knight;
		Knight* knight3 = new Knight;
		Knight* knight4 = new Knight;
		Viking* viking0 = new Viking;
		Viking* viking1 = new Viking;
		Viking* viking2 = new Viking;
		Viking* viking3 = new Viking;
		Viking* viking4 = new Viking;
		Wolf* wolf = new Wolf;
		HealthPoints* hp_enemy = new HealthPoints;
		HealthPoints* hp_enemy1 = new HealthPoints;
		HealthPoints* hp_enemy2 = new HealthPoints;
		HealthPoints* hp_enemy3 = new HealthPoints;
		HealthPoints* hp_enemy4 = new HealthPoints;
		HealthPoints* hp_enemy5 = new HealthPoints;
		HealthPoints* hp_enemy6 = new HealthPoints;
		HealthPoints* hp_enemy7 = new HealthPoints;
		HealthPoints* hp_knight = new HealthPoints;
		HealthPoints* hp_knight1 = new HealthPoints;
		HealthPoints* hp_knight2 = new HealthPoints;
		HealthPoints* hp_knight3 = new HealthPoints;
		HealthPoints* hp_knight4 = new HealthPoints;
		HealthPoints* hp_viking0 = new HealthPoints;
		HealthPoints* hp_viking1 = new HealthPoints;
		HealthPoints* hp_viking2 = new HealthPoints;
		HealthPoints* hp_viking3 = new HealthPoints;
		HealthPoints* hp_viking4 = new HealthPoints;
		HealthPoints* hp_wolf = new HealthPoints;

		if (enemy->newInit(&m_player, &m_fire, hp_enemy))
		{
			enemy->set_position(vec2{ 0,1600.f });
			m_enemies.emplace_back(enemy);
			hp_enemy->init(enemy->get_position(), { 1,0 }, enemy->get_hp(), enemy->largestHP);
			m_hps.emplace_back(hp_enemy);

			enemy->rigidbody = b2world->CreateBody(&enemy->skullBodyDef);
			enemy->rigidbody->CreateFixture(&enemy->skullFixtureDef);
			enemy->rigidbody->SetUserData(enemy);



		}
		if (enemy1->newInit(&m_player, &m_fire, hp_enemy1))
		{
			enemy1->set_position(vec2{ -500,1600.f });
			m_enemies.emplace_back(enemy1);
			hp_enemy1->init(enemy->get_position(), { 1,0 }, enemy->get_hp(), enemy1->largestHP);
			m_hps.emplace_back(hp_enemy1);


			enemy1->rigidbody = b2world->CreateBody(&enemy1->skullBodyDef);
			enemy1->rigidbody->CreateFixture(&enemy1->skullFixtureDef);
			enemy1->rigidbody->SetUserData(enemy1);
		}
		if (enemy2->newInit(&m_player, &m_fire, hp_enemy2))
		{
			enemy2->set_position(vec2{ -100,1600.f });
			m_enemies.emplace_back(enemy2);
			hp_enemy2->init(enemy2->get_position(), { 1,0 }, enemy2->get_hp(),enemy2->largestHP);
			m_hps.emplace_back(hp_enemy2);


			enemy2->rigidbody = b2world->CreateBody(&enemy2->skullBodyDef);
			enemy2->rigidbody->CreateFixture(&enemy2->skullFixtureDef);
			enemy2->rigidbody->SetUserData(enemy2);
		}
		if (enemy3->newInit(&m_player, &m_fire, hp_enemy3))
		{
			enemy3->set_position(vec2{ -3000,1600.f });
			m_enemies.emplace_back(enemy3);
			hp_enemy3->init(enemy3->get_position(), { 1,0 }, enemy3->get_hp(),enemy3->largestHP);
			m_hps.emplace_back(hp_enemy3);


			enemy3->rigidbody = b2world->CreateBody(&enemy3->skullBodyDef);
			enemy3->rigidbody->CreateFixture(&enemy3->skullFixtureDef);
			enemy3->rigidbody->SetUserData(enemy3);
		}

		if (enemy4->newInit(&m_player, &m_fire, hp_enemy4))
		{
			enemy4->set_position(vec2{ -1000,1600.f });
			m_enemies.emplace_back(enemy4);
			hp_enemy4->init(enemy4->get_position(), { 1,0 }, enemy4->get_hp(), enemy4->largestHP);
			m_hps.emplace_back(hp_enemy4);


			enemy4->rigidbody = b2world->CreateBody(&enemy4->skullBodyDef);
			enemy4->rigidbody->CreateFixture(&enemy4->skullFixtureDef);
			enemy4->rigidbody->SetUserData(enemy4);
		}
		if (enemy5->newInit(&m_player, &m_fire, hp_enemy5))
		{
			enemy5->set_position(vec2{ 1000,1600.f });
			m_enemies.emplace_back(enemy5);
			hp_enemy5->init(enemy5->get_position(), { 1,0 }, enemy5->get_hp(),enemy5->largestHP);
			m_hps.emplace_back(hp_enemy5);


			enemy5->rigidbody = b2world->CreateBody(&enemy5->skullBodyDef);
			enemy5->rigidbody->CreateFixture(&enemy5->skullFixtureDef);
			enemy5->rigidbody->SetUserData(enemy5);
		}
		if (enemy6->newInit(&m_player, &m_fire, hp_enemy6))
		{
			enemy6->set_position(vec2{ 2000,1600.f });
			m_enemies.emplace_back(enemy6);
			hp_enemy6->init(enemy6->get_position(), { 1,0 }, enemy6->get_hp(), enemy6->largestHP);
			m_hps.emplace_back(hp_enemy6);


			enemy6->rigidbody = b2world->CreateBody(&enemy6->skullBodyDef);
			enemy6->rigidbody->CreateFixture(&enemy6->skullFixtureDef);
			enemy6->rigidbody->SetUserData(enemy6);
		}
		if (enemy7->newInit(&m_player, &m_fire, hp_enemy7))
		{
			enemy7->set_position(vec2{ 1500,1600.f });
			m_enemies.emplace_back(enemy7);
			hp_enemy7->init(enemy7->get_position(), { 1,0 }, enemy7->get_hp(), enemy7->largestHP);
			m_hps.emplace_back(hp_enemy7);


			enemy7->rigidbody = b2world->CreateBody(&enemy7->skullBodyDef);
			enemy7->rigidbody->CreateFixture(&enemy7->skullFixtureDef);
			enemy7->rigidbody->SetUserData(enemy7);
		}

		for (int i = 0; i < 8; i++) {

			//Skull* skull = (Skull*)m_enemies[i];

			
			AABB* skull_ab = coll.Abinit(m_enemies[i]->vertex[2], m_enemies[i]->vertex[0], enemy_a);
			coll.addverindices(skull_ab, m_enemies[i]->indicess);
			coll.addvertex(skull_ab, m_enemies[i]->vertex);
			skull_ab->enemy = m_enemies[i];
			aabbs.emplace_back(skull_ab);
			

			/*
			skull->rigidbody = b2world->CreateBody(&skull->skullBodyDef);
			skull->rigidbody->CreateFixture(&skull->skullFixtureDef);
			skull->rigidbody->SetUserData(skull);
			*/

		}

		if (viking0->newInit(&m_player, &m_fire, hp_viking0))
		{
			viking0->set_position(vec2{ 200,1600.f });
			m_enemies.emplace_back(viking0);
			hp_viking0->init(viking0->get_position(), { 1,0 }, viking0->get_hp(), viking0->largestHP);
			m_hps.emplace_back(hp_viking0);


			viking0->rigidbody = b2world->CreateBody(&viking0->vikingBodyDef);
			viking0->rigidbody->CreateFixture(&viking0->vikingFixtureDef);
			viking0->rigidbody->SetUserData(viking0);
		}
		if (viking1->newInit(&m_player, &m_fire, hp_viking1))
		{
			viking1->set_position(vec2{ 700,1600.f });
			m_enemies.emplace_back(viking1);
			hp_viking1->init(viking1->get_position(), { 1,0 }, viking1->get_hp(), viking1->largestHP);
			m_hps.emplace_back(hp_viking1);

			viking1->rigidbody = b2world->CreateBody(&viking1->vikingBodyDef);
			viking1->rigidbody->CreateFixture(&viking1->vikingFixtureDef);
			viking1->rigidbody->SetUserData(viking1);
		}
		if (viking2->newInit(&m_player, &m_fire, hp_viking2))
		{
			viking2->set_position(vec2{ 300,1600.f });
			m_enemies.emplace_back(viking2);
			hp_viking2->init(viking2->get_position(), { 1,0 }, viking2->get_hp(), viking2->largestHP);
			m_hps.emplace_back(hp_viking2);

			viking2->rigidbody = b2world->CreateBody(&viking2->vikingBodyDef);
			viking2->rigidbody->CreateFixture(&viking2->vikingFixtureDef);
			viking2->rigidbody->SetUserData(viking2);
		}
		if (viking3->newInit(&m_player, &m_fire, hp_viking3))
		{
			viking3->set_position(vec2{ 1700,1600.f });
			m_enemies.emplace_back(viking3);
			hp_viking3->init(viking3->get_position(), { 1,0 }, viking3->get_hp(), viking3->largestHP);
			m_hps.emplace_back(hp_viking3);

			viking3->rigidbody = b2world->CreateBody(&viking3->vikingBodyDef);
			viking3->rigidbody->CreateFixture(&viking3->vikingFixtureDef);
			viking3->rigidbody->SetUserData(viking3);
		}

		if (viking4->newInit(&m_player, &m_fire, hp_viking4))
		{
			viking4->set_position(vec2{ -1200,1600.f });
			m_enemies.emplace_back(viking4);
			hp_viking4->init(viking4->get_position(), { 1,0 }, viking4->get_hp(), viking4->largestHP);
			m_hps.emplace_back(hp_viking4);

			viking4->rigidbody = b2world->CreateBody(&viking4->vikingBodyDef);
			viking4->rigidbody->CreateFixture(&viking4->vikingFixtureDef);
			viking4->rigidbody->SetUserData(viking4);
		}


		for (int i = 8; i < 13; i++) {
			
			AABB* viking_ab = coll.Abinit(m_enemies[i]->vertex[2], m_enemies[i]->vertex[0], enemy_a);
			coll.addverindices(viking_ab, m_enemies[i]->indicess);
			coll.addvertex(viking_ab, m_enemies[i]->vertex);
			viking_ab->enemy = m_enemies[i];
			aabbs.emplace_back(viking_ab);
			
			/*
			Viking* viking = (Viking*)m_enemies[i];

			viking->rigidbody = b2world->CreateBody(&viking->vikingBodyDef);
			viking->rigidbody->CreateFixture(&viking->vikingFixtureDef);
			viking->rigidbody->SetUserData(viking);*/

		}

		if (knight->newInit(&m_player, &m_fire, hp_knight))
		{

			knight->set_position(vec2{ 2400, 1600.f });
			m_enemies.emplace_back(knight);
			hp_knight->init(knight->get_position(), { 1,0 }, knight->get_hp(), knight->largestHP);
			m_hps.emplace_back(hp_knight);


			knight->rigidbody = b2world->CreateBody(&knight->knightBodyDef);
			knight->rigidbody->CreateFixture(&knight->knightFixtureDef);
		    knight->rigidbody->SetUserData(knight);
		}

		if (knight1->newInit(&m_player, &m_fire, hp_knight1))
		{

			knight1->set_position(vec2{ 2600, 1600.f });
			m_enemies.emplace_back(knight1);
			hp_knight1->init(knight1->get_position(), { 1,0 }, knight1->get_hp(), knight1->largestHP);
			m_hps.emplace_back(hp_knight1);

			knight1->rigidbody = b2world->CreateBody(&knight1->knightBodyDef);
			knight1->rigidbody->CreateFixture(&knight1->knightFixtureDef);
			knight1->rigidbody->SetUserData(knight1);
		}

		if (knight2->newInit(&m_player, &m_fire, hp_knight2))
		{

			knight2->set_position(vec2{ 2900, 1600.f });
			m_enemies.emplace_back(knight2);
			hp_knight2->init(knight2->get_position(), { 1,0 }, knight2->get_hp(), knight2->largestHP);
			m_hps.emplace_back(hp_knight2);

			knight2->rigidbody = b2world->CreateBody(&knight2->knightBodyDef);
			knight2->rigidbody->CreateFixture(&knight2->knightFixtureDef);
			knight2->rigidbody->SetUserData(knight2);
		}

		if (knight3->newInit(&m_player, &m_fire, hp_knight3))
		{

			knight3->set_position(vec2{ 3200, 1600.f });
			m_enemies.emplace_back(knight3);
			hp_knight3->init(knight3->get_position(), { 1,0 }, knight3->get_hp(), knight3->largestHP);
			m_hps.emplace_back(hp_knight3);

			knight3->rigidbody = b2world->CreateBody(&knight3->knightBodyDef);
			knight3->rigidbody->CreateFixture(&knight3->knightFixtureDef);
			knight3->rigidbody->SetUserData(knight3);
		}

		if (knight4->newInit(&m_player, &m_fire, hp_knight4))
		{

			knight4->set_position(vec2{ 3500, 1600.f });
			m_enemies.emplace_back(knight4);
			hp_knight4->init(knight4->get_position(), { 1,0 }, knight4->get_hp(), knight4->largestHP);
			m_hps.emplace_back(hp_knight4);

			knight4->rigidbody = b2world->CreateBody(&knight4->knightBodyDef);
			knight4->rigidbody->CreateFixture(&knight4->knightFixtureDef);
			knight4->rigidbody->SetUserData(knight4);
		}

		for (int i = 13; i < 18; i++) {

			
			AABB* knight_ab = coll.Abinit(m_enemies[i]->vertex[2], m_enemies[i]->vertex[0], enemy_a);
			coll.addverindices(knight_ab, m_enemies[i]->indicess);
			coll.addvertex(knight_ab, m_enemies[i]->vertex);
			knight_ab->enemy = m_enemies[i];
			aabbs.emplace_back(knight_ab);
			
			/*
			Knight* knight = (Knight*)m_enemies[i];

			knight->rigidbody = b2world->CreateBody(&knight->knightBodyDef);
			knight->rigidbody->CreateFixture(&knight->knightFixtureDef);
			knight->rigidbody->SetUserData(knight);
			*/
		}

		if (wolf->newInit(&m_player, &m_fire, hp_wolf))
		{
			wolf->set_position(vec2{ 4000,1600.f });
			m_enemies.emplace_back(wolf);
			hp_wolf->init(wolf->get_position(), { 1,0 }, wolf->get_hp(), wolf->largestHP);
			m_hps.emplace_back(hp_wolf);

			
			AABB* wolf_ab = coll.Abinit(wolf->vertex[2], wolf->vertex[0], enemy_a);
			coll.addverindices(wolf_ab, wolf->indicess);
			coll.addvertex(wolf_ab, wolf->vertex);
			wolf_ab->enemy = wolf;
			aabbs.emplace_back(wolf_ab);
			

			
			wolf->rigidbody = b2world->CreateBody(&wolf->wolfBodyDef);
			wolf->rigidbody->CreateFixture(&wolf->wolfFixtureDef);
			wolf->rigidbody->SetUserData(wolf);
			

		}
		
	}

	if(game_level == 1){
		Dragon* dragon = new Dragon;
		Skull* skull1 = new Skull;
		Knight* knight1 = new Knight;
		Knight* knight2 = new Knight;


		
		
		HealthPoints* hp_dragon = new HealthPoints;
		HealthPoints* hp_skull1 = new HealthPoints;
		HealthPoints* hp_knight1 = new HealthPoints;
		HealthPoints* hp_knight2 = new HealthPoints;
		

		if (skull1->newInit(&m_player, &m_fire, hp_skull1))
		{
			skull1->set_position(vec2{ -3000,380.f });
			//skull1->set_position(vec2{ 4400,0.f });
			m_enemies.emplace_back(skull1);
			hp_skull1->init(skull1->get_position(), { 1,0 }, skull1->get_hp(), skull1->largestHP);
			m_hps.emplace_back(hp_skull1);

			
			AABB* skull1_ab = coll.Abinit(skull1->vertex[2], skull1->vertex[0], enemy_a);
			coll.addverindices(skull1_ab, skull1->indicess);
			coll.addvertex(skull1_ab, skull1->vertex);
			skull1_ab->enemy = skull1;
			aabbs.emplace_back(skull1_ab);
			
			
			skull1->rigidbody = b2world->CreateBody(&skull1->skullBodyDef);
			skull1->rigidbody->CreateFixture(&skull1->skullFixtureDef);
			skull1->rigidbody->SetUserData(skull1);
			
		}

		if (knight1->newInit(&m_player, &m_fire, hp_knight1))
		{

			knight1->set_position(vec2{ -4800, 300.f });
			m_enemies.emplace_back(knight1);
			hp_knight1->init(knight1->get_position(), { 1,0 }, knight1->get_hp(), knight1->largestHP);
			m_hps.emplace_back(hp_knight1);


			   
				AABB* knight_ab = coll.Abinit(knight1->vertex[2], knight1->vertex[0], enemy_a);
				coll.addverindices(knight_ab, knight1->indicess);
				coll.addvertex(knight_ab, knight1->vertex);
				knight_ab->enemy = knight1;
				aabbs.emplace_back(knight_ab);
				
				
			   
				knight1->rigidbody = b2world->CreateBody(&knight1->knightBodyDef);
				knight1->rigidbody->CreateFixture(&knight1->knightFixtureDef);
				knight1->rigidbody->SetUserData(knight1);
				
		}

		if (knight2->newInit(&m_player, &m_fire, hp_knight2))
		{

			knight2->set_position(vec2{ 2200, 0.f });
			m_enemies.emplace_back(knight2);
			hp_knight2->init(knight2->get_position(), { 1,0 }, knight2->get_hp(), knight2->largestHP);
			m_hps.emplace_back(hp_knight2);


			
			AABB* knight_ab = coll.Abinit(knight1->vertex[2], knight1->vertex[0], enemy_a);
			coll.addverindices(knight_ab, knight1->indicess);
			coll.addvertex(knight_ab, knight1->vertex);
			knight_ab->enemy = knight2;
			aabbs.emplace_back(knight_ab);
			
			
			knight2->rigidbody = b2world->CreateBody(&knight2->knightBodyDef);
			knight2->rigidbody->CreateFixture(&knight2->knightFixtureDef);
			knight2->rigidbody->SetUserData(knight2);
			
		}



		if (dragon->newInit(&m_player, &m_fire, hp_dragon))
		{
			dragon->set_position(vec2{ 5800.f,-200.f });
			m_enemies.emplace_back(dragon);
			hp_dragon->init(dragon->get_position(), { 1,0 }, dragon->get_hp(), dragon->largestHP);
			m_hps.emplace_back(hp_dragon);

			
			AABB* dragon_ab = coll.Abinit(dragon->vertex[2], dragon->vertex[0], enemy_a);
			coll.addverindices(dragon_ab, dragon->indicess);
			coll.addvertex(dragon_ab, dragon->vertex);
			dragon_ab->enemy = dragon;
			aabbs.emplace_back(dragon_ab);
			
			
			dragon->rigidbody = b2world->CreateBody(&dragon->dragonBodyDef);
			dragon->rigidbody->CreateFixture(&dragon->dragonFixtureDef);
			dragon->rigidbody->SetUserData(dragon);
			

		}
	}
	return true;
}

// Creates a new npc and if successfull adds it to the list of npc

void World::bounce(float top, float bot, float left, float right) {
	if (m_player.is_alive()) {
		if (m_player.get_position().y < top + 50)
			m_player.move({ 0.f,6.8f });
		if (m_player.get_position().x < left + 50)
			m_player.move({6.8f,0.f});
		if (m_player.get_position().x > right - 50)
			m_player.move({ -6.8f,0.f });
	}
}

void World::update_allphysics(float ms) {
	m_player.physic.update(m_current_speed*ms);
	for (auto& e : m_enemies) {
		e->physic.update(m_current_speed*ms);
	}
}

void World::playerVsenemy(Player * p1, Enemy * e1)
{
	if (e1->get_hp() > 0) {
		if (countDown == 0) {
		
			{
				countDown = 70;
				if (m_player.deadOrNot()) {
					if (m_player.is_alive()) {
						Mix_PlayChannel(-1, m_player_dead_sound, 0);
						m_frontGround.set_player_dead();
					}
					m_player.kill();
				}
			
			}
		}

	}

}

// On key callback
void World::on_key(GLFWwindow*, int key, int a, int action, int mod)
{
	if (start_show) {
		if (action == GLFW_PRESS && key == GLFW_KEY_DOWN) {
			m_start.decrease_pages();
			return;
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_UP) {
			m_start.increase_pages();
			return;
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && m_start.get_page() == 0) {
			start_show = false;
			b2Vec2 force = b2Vec2(20, 50000);
			ballBody->ApplyLinearImpulse(force, ballBody->GetPosition(), true);
			//ballBody->SetTransform(point, 65.0f);
			return;
		}
		return;
	}

	if (!m_player.is_alive()) {
		return;
	}
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE player MOVEMENT HERE
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	////////////////////////////Helper/////////////////////////////////////

	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
		if (helper_show) helper_show = false;
		else helper_show = true;
	}
	if (helper_show) {
		if (action == GLFW_PRESS && key == GLFW_KEY_LEFT) {
			m_helper.decrease_pages();
			return;
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT) {
			m_helper.increase_pages();
			return;
		}
	}

	///////////////////////////Orb Jumping/////////////////////////////////////
	if (!orb_jumping && can_orb_jumping) {
		if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
			orb_jumping = true;
			m_arrow.set_angle(0.f);
			return;
		}
	}
	if (orb_jumping ) {
		if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
			m_player.physic.one_time_force = m_arrow.get_jumping_force();
			m_player.physic.motion = vec2({0.f, 0.f});
			m_player.physic.orb_jump = 300.f;
			orb_jumping = false;
			return;
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_UP) {
			m_arrow.rotate(-PI/12.f);
			return;
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_DOWN) {
			m_arrow.rotate(PI / 12.f);
			return;
		}
		if (action == GLFW_RELEASE && key == GLFW_KEY_LEFT)                    // in player, 1 means left; 2means right; 3 means jump
			m_player.set_Direction(1, false);
		if (action == GLFW_RELEASE && key == GLFW_KEY_RIGHT)
			m_player.set_Direction(2, false);
		if (action == GLFW_PRESS && key == GLFW_KEY_LEFT) {
			m_player.set_Direction(1, true);
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT) {
			m_player.set_Direction(2, true);
		}
		return;
	}

	///////////////////////////////////////////////////////////////
	if (action == GLFW_RELEASE && key == GLFW_KEY_LEFT)                    // in player, 1 means left; 2means right; 3 means jump
		m_player.set_Direction(1, false);
	if (action == GLFW_RELEASE && key == GLFW_KEY_RIGHT)
		m_player.set_Direction(2, false);
	if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE) {
		m_player.set_Direction(3, false);
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_S) {
		WorldSave();
		return;
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_L) {
		WorldLoad();
		return;
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_P)
		m_current_speed = m_current_speed == 0.f ? 1.f : 0.f;
	if (m_current_speed == 0) return;
	if (action == GLFW_RELEASE && key == GLFW_KEY_Z) {
		m_player.attack(false);
	}


	if (action == GLFW_RELEASE && key == GLFW_KEY_X) {
		m_player.cast_shield(false);
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_C) {
		m_player.cast_flare(false);
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_Z) {
	Attack_P* attack_c = m_player.attack(true);
	if (!attack_c) return;
	m_attack_ps.emplace_back(attack_c);
	
	if (attack_c != NULL && attack_c->created == true) {

		/*
		AABB* attack_ab = coll.Abinit(attack_c->vertex[2], attack_c->vertex[0], attack_a);
		coll.addverindices(attack_ab, attack_c->indicess);
		coll.addvertex(attack_ab, attack_c->vertex);
		attack_ab->attack = attack_c;
		aabbs.emplace_back(attack_ab);
		*/


		attack_c->setDef();
		attack_c->rigidbody = b2world->CreateBody(&attack_c->attackBodyDef);
		attack_c->rigidbody->CreateFixture(&attack_c->attackFixtureDef);
		attack_c->rigidbody->SetUserData(attack_c);

	}
	

    }

	if (action == GLFW_PRESS && key == GLFW_KEY_C)
	{
		Flare_P* flare_c = m_player.cast_flare(true);

		if (flare_c == NULL) {

		}
		else {
			if (flare_c->created) {
				m_flare_ps.emplace_back(flare_c);
				/*
				AABB* flare_ab = coll.Abinit(flare_c->vertex[2], flare_c->vertex[0], flare_a);
				coll.addverindices(flare_ab, flare_c->indicess);
				coll.addvertex(flare_ab, flare_c->vertex);
				flare_ab->flare = flare_c;
				aabbs.emplace_back(flare_ab);*/

				flare_c->setDef();
				flare_c->rigidbody = b2world->CreateBody(&flare_c->flareBodyDef);
				flare_c->rigidbody->CreateFixture(&flare_c->flareFixtureDef);
				flare_c->rigidbody->SetUserData(flare_c);
			}

		}
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_V)
	{
		soul_chain.activated = false;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_V)
	{
		if (soul_chain.in_range && soul_chain.enough_lv) {
			soul_chain.activated = true;
			soul_chain.start();
		}
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_1) {
		m_player.use_item(1);
	}

	/*if (action == GLFW_PRESS && key == GLFW_KEY_Z) {
			Shield_P* shield = m_player.cast_shield(true);
			if(shield->player) m_shield_ps.emplace_back(shield);


	}*/
	if (action == GLFW_PRESS && key == GLFW_KEY_F1) {
		game_level--;
		game_level = game_level > 0 ? game_level : 0;
		scene_change();
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_F2) {
		game_level++;
		game_level = game_level < level_size ? game_level : level_size-1;
		scene_change();
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_X) {
		Shield_P* shield_c = m_player.cast_shield(true);
		if (shield_c == NULL) { return; }
		if (shield_c->player) { m_shield_ps.emplace_back(shield_c); }

		if (shield_c->created) {

			/*
			AABB* shield_ab = coll.Abinit(shield_c->vertex[2], shield_c->vertex[0], shield_a);
			coll.addverindices(shield_ab, shield_c->indicess);
			coll.addvertex(shield_ab, shield_c->vertex);
			shield_ab->shield = shield_c;
			aabbs.emplace_back(shield_ab);
            */


			shield_c->setDef();
			shield_c->rigidbody = b2world->CreateBody(&shield_c->shieldBodyDef);
			shield_c->rigidbody->CreateFixture(&shield_c->shieldFixtureDef);
			shield_c->rigidbody->SetUserData(shield_c);

		}


	}
	if (action == GLFW_PRESS && key == GLFW_KEY_LEFT) {
			m_player.set_Direction(1, true);
		}
	if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT) {
			m_player.set_Direction(2, true);
		}
	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
		m_player.set_Direction(3, true);
		
		//m_player.onCube = false;

	}
	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		int w, h;
		glfwGetWindowSize(m_window, &w, &h);
		clear();
		spawn();
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) &&  key == GLFW_KEY_COMMA)
		m_current_speed -= 0.1f;
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
		m_current_speed += 0.1f;

	m_current_speed = fmax(0.f, m_current_speed);
}


void World::on_mouse_move(GLFWwindow* window, int button, int action, int mods)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE quest_manager size here
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		quest_manager.enlarge = !quest_manager.enlarge;
}

void World::scene_change() {
	clear();
	spawn();
	//TODO: like reset
	
	switch (game_level)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	}
}

void World::clear() {
	m_frontGround.destroy();
	m_backGround.destroy();
	for (auto& a : m_enemies) {
		a->destroy();
		delete a;
	}
	for (auto& a : m_attack_ps) {
		a->destroy();
		delete a;
	}
	for (auto& a : m_shield_ps) {
		a->destroy();
		delete a;
	}
	for (auto& a : m_flare_ps) {
		a->destroy();
		delete a;
	}
	for (auto& a : m_fire) {
		a.destroy();
		//delete &a;
	}
	for (auto& a : m_cross) {
		a->destroy();
		delete a;
	}
	for (auto& a : m_mace) {
		a->destroy();
		delete a;
	}
	for (auto& a : m_venom) {
		a->destroy();
		delete a;
	}
	for (auto& a : m_swamp) {
		a->destroy();
		delete a;
	}
	for (auto& a : m_pillar) {
		a->destroy();
		delete a;
	}
	for (auto& a : m_chest) {
		a->destroy();
		delete a;
	}
	for (auto& a : goldenorbs) {
		a->destroy();
		delete a;
	}
	for (auto& hp : m_hps) {
		hp->destroy();
		delete hp;
	}
	for (auto& g : grounds) {
		g->destroy();
		delete g;
	}
	for (auto& g : jumporbs) {
		g->destroy();
		delete g;
	}
	for (auto& s : stones) {
		s->destroy();
		delete s;
	}
	for (auto& a : dragon_fires) {
		a->destroy();
		delete a;
	}
	for (auto& a : dragon_thunder) {
		a->destroy();
		delete a;
	}
	
	for (auto a : aabbs) {
		delete a;
	}
	
	quest_manager.destroy();
	soul_chain.destroy();
	m_npc.destroy();
	m_enemies.clear();
	m_hps.clear();
	m_attack_ps.clear();
	m_shield_ps.clear();
	m_flare_ps.clear();
	m_fire.clear();
	m_cross.clear();
	m_mace.clear();
	m_venom.clear();
	m_swamp.clear();
	m_pillar.clear();
	m_chest.clear();
	grounds.clear();
	stones.clear();
	jumporbs.clear();
	goldenorbs.clear();
	dragon_fires.clear();
	dragon_thunder.clear();
	aabbs.clear();
	delete b2world;
}

void World::spawn() {
	m_frontGround.frontGround = screen_texs[game_level];
	m_backGround.backGround = background_tex[game_level];
	m_frontGround.init();
	m_backGround.init();
	m_frontGround.reset_player_dead_time();
	m_current_speed = 1.f;
	b2world = new b2World(gravity);
	b2world->SetContactListener(&myContactListenerInstance);

	fireball.setDef();
	fireball.rigidbody = b2world->CreateBody(&(fireball.fireBallBodyDef));
	fireball.rigidbody->CreateFixture(&(fireball.fireBallFixtureDef));
	fireball.rigidbody->SetUserData(&fireball);

	quest_manager.init(&m_player);
	m_player.b2world = b2world;
	switch (game_level)
	{
	case 0:
		m_player.reset(vec2{ -4000,1600},0, 1);
		m_player.set_height_limit(1600.f);
		quest_manager.current = 0;
		fireball.reset();
		break;
	case 1:
		//m_player.reset(vec2{ -3400,1000 });
		m_player.reset(vec2{ -6000,300 }, 3500, 7);
		//m_player.reset(vec2{ 5000,800 });
		//m_player.reset(vec2{ 4800,-200 });
		m_player.set_height_limit(1800.f);
		m_npc.init();
		m_npc.set_position({ 1977.f, 230.f });
		quest_manager.current = 4;
		break;
	case 2:
		m_player.reset(vec2{ -6000,-100 }, 3500, 7);
		//m_player.reset(vec2{ 1500,380 }, 275, 6);
		//m_player.reset(vec2{ -1000,50 },275,6);
		//m_player.reset(vec2{ -3000,-100 });
		//m_player.reset(vec2{ 2400,970 });
		//m_player.reset(vec2{ 1000,350 });
		//m_player.reset(vec2{ 4100,380 });
		m_player.set_height_limit(1800.f);
		quest_manager.current = 3;
	}
	HealthPoints* hp_player = new HealthPoints;
	hp_player->init(m_player.get_position(), { 1,0 }, m_player.get_hp(), 10);
	m_player.setHPbar(hp_player);
	m_hps.emplace_back(hp_player);

	
	AABB* player_ab = coll.Abinit(m_player.vertex[2], m_player.vertex[0], player_a);
	coll.addverindices(player_ab, m_player.indicess);
	coll.addvertex(player_ab, m_player.vertex);
	player_ab->player = &m_player;
	aabbs.emplace_back(player_ab);

	m_player.rigidbody = b2world->CreateBody(&m_player.playerBodyDef);
	m_player.rigidbody->CreateFixture(&m_player.playerFixtureDef);
	m_player.rigidbody->SetUserData(&m_player);
	soul_chain.init(&m_player, NULL);

	spawn_fires();
	spawn_mace();
	spawn_venom();
	spawn_swamp();
	spawn_pillar();
	spawn_cross();
	spawn_enemy();
	spawn_grounds();
	spawn_stones();
	spawn_jumporbs();
	spawn_goldenorbs();
	spawn_chest();
	//m_arrow.init({ 0.f, 0.f });
	//m_player.m_position = vec2{ -3000, 1200.f };
	checked = 0;

}

