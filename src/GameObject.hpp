#pragma once

// stlib
#include <fstream> // stdout, stderr..

// glfw
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// Simple utility macros to avoid mistyping directory name, name has to be a string literal
// audio_path("audio.ogg") -> data/audio/audio.ogg
// Get defintion of PROJECT_SOURCE_DIR from:
#include "project_path.hpp"
//#include "vld.h"

#define shader_path(name) PROJECT_SOURCE_DIR "./shaders/" name
#define data_path PROJECT_SOURCE_DIR "./data"
#define textures_path(name)  data_path "/textures/" name
#define character_stand data_path "/textures/playerAnimationTex/IceWizard/Stand/"
#define character_run data_path "/textures/playerAnimationTex/IceWizard/Run/"
#define character_jump data_path "/textures/playerAnimationTex/IceWizard/Jump/"
#define character_down data_path "/textures/playerAnimationTex/IceWizard/Down/"
#define character_hit data_path "/textures/playerAnimationTex/IceWizard/Hit/"
#define character_die data_path "/textures/playerAnimationTex/IceWizard/Die/"
#define character_cast data_path "/textures/playerAnimationTex/IceWizard/Cast1H/"
#define character_attack data_path "/textures/playerAnimationTex/skill/attack_p/"
#define character_shield data_path "/textures/playerAnimationTex/skill/shield/"
#define character_flare data_path "/textures/playerAnimationTex/skill/flare/"
#define character_levelup data_path "/textures/playerAnimationTex/level_up/"
#define knight_run data_path "/textures/enemy/Knight/Run/"
#define knight_attack data_path "/textures/enemy/Knight/Attack1H/"
#define viking_run data_path "/textures/enemy/Viking1/Run/"
#define viking_attack data_path "/textures/enemy/Viking1/Attack1H/"
#define skull_run data_path "/textures/enemy/Skull/Run/"
#define skull_attack data_path "/textures/enemy/Skull/Attacking/"
#define wolf_run data_path "/textures/enemy/greyWolf/run/"
#define wolf_attack data_path "/textures/enemy/greyWolf/bite/"
#define wolf_jump data_path "/textures/enemy/greyWolf/jump/"
#define dragon_idle data_path "/textures/enemy/Dragon/Idle/"
#define dragon_sleep data_path "/textures/enemy/Dragon/Idle/10.png"
#define dragon_attack data_path "/textures/enemy/Dragon/Attack/"
#define dragon_fly data_path "/textures/enemy/Dragon/Fly/"
#define fire_e data_path "/textures/enemy/Dragon/Fire/"
#define thunder_e data_path "/textures/enemy/Dragon/Magic/"
#define ground_fire data_path "/textures/surroundings/fire/"
#define ground_fireball data_path "/textures/surroundings/fireball/"
#define ground_jumporb data_path "/textures/jumporb/"
#define ground_goldenorb data_path "/textures/goldenorb/"
#define ground_mace data_path "/textures/mace/"
#define ground_venom data_path "/textures/poison/"
#define ground_swamp data_path "/textures/swamp/"
#define ground_pillar data_path "/textures/pillar/"
#define ground_chest data_path "/textures/chest/"
#define ground_arrow data_path "/textures/jumporb/arrow.png"
#define ground_smoke data_path "/textures/surroundings/smoke/"
#define blood_path data_path "/textures/surroundings/blood/"
#define backGround0 data_path "/textures/background/1.png"
#define backGround1 data_path "/textures/background/2.png"
#define backGround2 data_path "/textures/background/3.png"
#define r_backGround2 data_path "/textures/background/03.png"
#define r_backGround1 data_path "/textures/background/02.png"
#define r_backGround0 data_path "/textures/background/01.png"
#define ground1 data_path "/textures/ground/1.png"
#define normal_particle data_path "/textures/particles/1.png"
#define stone data_path "/textures/stone/stone.png"
#define helper data_path "/textures/helper/"
#define starter data_path "/textures/start/"
#define keyui data_path "/textures/keyui/"
#define potion data_path "/textures/item/potion.png"
#define quest data_path "/textures/quest/"
#define quest0 data_path "/textures/quest/0.png"
#define questmanager data_path "/textures/questmanager/"
#define questmanager0 data_path "/textures/questmanager/0.png"
#define icon_shield data_path "/textures/surroundings/icons/shield/"
#define icon_flare data_path "/textures/surroundings/icons/flare/"
#define icon_potion data_path "/textures/surroundings/icons/potion/"
#define icon_soulchain data_path "/textures/surroundings/icons/soul/"
#define full_blood data_path "/textures/surroundings/overlay/"
#define npc data_path "/textures/playerAnimationTex/Archer/Stand/"
#define npc0 data_path "/textures/playerAnimationTex/Archer/Stand/0.png"
#define newquest data_path "/textures/newquest/"
#define newquest0 data_path "/textures/newquest/0.png"
#define soulchain data_path "/textures/playerAnimationTex/skill/SoulChain/"
#define  PI  3.14159265358979f
#define audio_path(name) data_path  "/audio/" name
#define mesh_path(name) data_path  "/meshes/" name
#define Global_frame_CountDown 5
#define Gloabal_animation_CountDown 60
#define FullBlood_Size 3
#define Global_gravity 0.58f
#define General_friction 5.93f
#define Global_jump_CountDown 40
#define Wolf_Global_jump_CountDown 90
#define player_run_stepMax 4.0f
#define player_jump_stepMax 80.0
#define Ground_fire_textures 13
#define player_normal_attack_cd 300
#define Ground_fire_textures 30
#define Ground_fireball_textures 10
#define Ground_smoke_textures 8
#define player_normal_shield_cd 5000
#define Ground_smoke_textures 16
#define Wolf_run_textures 16
#define blood_textures 11
#define player_normal_shield_cd 10000
#define player_normal_flare_cd 60000
#define venom_texture_number 7
#define pixeltob2 0.01f
#define b2topixel 100.0f

#define icon_size 3
#define icon_wait_timeout 1
#define icon_wait_levelup 0
#define icon_okay 2

#define fullblood_nromal 0
#define fullblood_severe 1
#define fullblood_critical 2

#define ET_player 5     //leading 0, entities belongs to player
#define ET_player_attack 1
#define ET_player_shield 2
#define ET_player_flare  3
#define ET_sleep_position 4

#define ET_skull  10    //leading 1, entities belongs to enemy
#define ET_viking 11
#define ET_kight  12
#define ET_wolf   13
#define ET_dragon 14
#define ET_dragon_fire 141
#define ET_dragon_thunder 142

#define ET_goldball 20   //leading 2, entities belong to pickable Item
#define ET_cross 21

#define ET_mace  30      //leading 3, entities belongs to interactive obstacles
#define ET_stone 320
#define ET_venom 32
#define ET_fire  33
#define ET_fire_ball 34
#define ET_ground 35
#define ET_jumporb 36
#define ET_swamp 37
#define ET_pillar 38
#define ET_chest 39


#define ET_background 90 //leading 9, system stuff(unsure)
#define ET_frontground 91
#define ET_helper 92
#define ET_start 93
#define ET_arrow 94
#define ET_keyui 95
#define ET_smoke 96
#define ET_level_up 97
#define ET_healthpoints 98
#define ET_particle 99


// Not much math is needed and there are already way too many libraries linked (:
// If you want to do some overloads..
struct vec2 { float x, y; };
struct vec3 { float x, y, z; };
struct mat3 { vec3 c0, c1, c2; };
struct vec4 { float x, y, z, a; };

// Utility functions
float dot(vec2 l, vec2 r);
float dot(vec3 l, vec3 r);
float dot(vec4 l, vec4 r);
mat3  mul(const mat3& l, const mat3& r);
vec2  normalize(vec2 v);
vec3  normalize(vec3 v);
vec4 normalize(vec4 v);

// OpenGL utilities
// cleans error buffer
void gl_flush_errors();
bool gl_has_errors();

// Single Vertex Buffer element for non-textured meshes (colored.vs.glsl)
struct Vertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Texture wrapper
struct Texture
{
	Texture();
	~Texture();

	GLuint id;
	GLuint depth_render_buffer_id;
	int width;
	int height;

	// Loads texture from file specified by path
	bool load_from_file(const char* path);
	// Screen texture
	bool create_from_screen(GLFWwindow const * const window, const char* path);
	bool is_valid()const; // True if texture is valid
};

// A Mesh is a collection of a VertexBuffer and an IndexBuffer. A VAO
// represents a Vertex Array Object and is the container for 1 or more Vertex Buffers and
// an Index Buffer
struct Mesh
{
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
};

// Container for Vertex and Fragment shader, which are then put(linked) together in a
// single program that is then bound to the pipeline.
struct Effect
{
	bool load_from_file(const char* vs_path, const char* fs_path);
	void release();

	GLuint vertex;
	GLuint fragment;
	GLuint program;
};

// Helper container for all the information we need when rendering an object together
// with its transform.
struct Renderable
{ 
	virtual ~Renderable() = default;

	virtual int getEntityType() = 0;
	virtual void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType) = 0;

	Mesh mesh;
	Effect effect;
	mat3 transform;

	// projection contains the orthographic projection matrix. As every Renderable::draw()
	// renders itself it needs it to correctly bind it to its shader.
	virtual void draw(const mat3& projection, mat3& view_2d) = 0;

	// gl Immediate mode equivalent, see the Rendering and Transformations section in the
	// specification pdf
	void transform_begin();
	void transform_scale(vec2 scale);
	void transform_rotate(float radians);
	void transform_translate(vec2 pos);
	void transform_mirror();
	void transform_updown();
	void transform_end();
};





//userdata for b2world
//struct bodyUserData{

//	int EntityType;
	/*
	0: Player
	1: Enemy
	2: Player_skills
	3: Enemy_attack(skills)
	4: Item (pickable)
	5: Interactive ostacle(jumporb, mace etc...)	
	*/
	
//	int EnemyType;
	/*
	works if EntityType == 1 , adding new types of enemy only change Enemytype
	0: skull
	1: viking
	2: kight
	3: wolf
	4: dragon
	*/

//	int SkillType;
	/*
	works if EntityType == 2 , adding new types of skills only change Skilltype
	0: Player_attack
	1: Player_shiled
	2: Player_flare
	*/

//	int EnemySkillType;
	/*
	works if EntityType == 3, adding new types of skills only change EnemySkillType
	0: Dragon_fire
	*/

//	int ItemType;
	/*
	works if EntityType == 4 , adding new types of items only change itemtype
	0: Cross
	1: goldballs
	*/

//	int ObstacleType;
	/*
	works if EntityType == 5 , adding new types of obstacles only change obstacletype
	0: JumpOrb
	1: Stone
	2: Venom
	3: Mace
    4: fire
	5: fireball
	*/
	
//};


