// Header
#include "player.hpp"

// internal
#include "enemy.hpp"
#include "npc.hpp"
#include "attack_p.hpp"
#include "Fire.hpp"
#include "Fireball.hpp"
#include "shield_p.hpp"
#include "flare_p.hpp"
#include "level_up.hpp"
#include "SleepPotion.hpp"
//#include "Collision.hpp"

// stlib
#include <vector>
#include <string>
#include <algorithm>

Texture Player::stand_textures[10];
Texture Player::run_textures[10];
Texture Player::jump_textures[10];
Texture Player::down_textures[10];
Texture Player::die_textures[10];
Texture Player::cast_textures[10];

Mix_Chunk* Player::m_player_lvlup_sound;
Mix_Chunk* Player::m_player_attack_sound;
Mix_Chunk* Player::m_player_shield_sound;
Mix_Chunk* Player::m_player_explosion_sound;
Mix_Chunk* Player::m_swamp;
Mix_Chunk* Player::m_swamp2;
bool Player::init()
{
	char texPath[] = character_run;
	char texName[] = "0.png";
	char buffer[256];
	strncpy(buffer, texPath, sizeof(buffer));
	strncat(buffer, texName, sizeof(buffer));
	player_texture.load_from_file(buffer);


	start_time = 0.0;
	run = false;
	cast = false;
	lifePoint = 10;
	total_exp = 0;
	level = 1;
	jump_countDown = Global_jump_CountDown;
	physic.init(true);
	/*
	// Reads the player mesh from a file, which contains a list of vertices and indices
	FILE* mesh_file = fopen(mesh_path("player.mesh"), "r");
	if (mesh_file == nullptr)
		return false;

	// Reading vertices and colors
	size_t num_vertices;
	fscanf(mesh_file, "%zu\n", &num_vertices);
	for (size_t i = 0; i < num_vertices; ++i)
	{
		float x, y, z;
		float _u[3]; // unused
		int r, g, b;
		fscanf(mesh_file, "%f %f %f %f %f %f %d %d %d\n", &x, &y, &z, _u, _u+1, _u+2, &r, &g, &b);
		Vertex vertex;
		vertex.position = { x, y, -z };
		vertex.color = { (float)r / 255, (float)g / 255, (float)b / 255 };
		vertices.push_back(vertex);
	}*/

	// The position corresponds to the center of the texture
	float wr = player_texture.width * 0.008f;
	float hr = player_texture.height * 0.008f;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.01f };
	vertices[1].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.01f };
	vertices[0].texcoord = { 1.f, 1.f, };
	vertices[2].position = { +wr, -hr, -0.01f };
	vertices[3].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.01f };
	vertices[2].texcoord = { 0.f, 0.f };

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };
	indicess = { indices[0],indices[1],indices[2],indices[3],indices[4],indices[5] };



	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	// Setting initial values
	m_scale.x = -35.f;
	m_scale.y = 35.f;
	m_is_alive = true;
	shielded = false;
	//shield_countDown = 0;
	m_position = { -4000.f, 1600.f };
	m_rotation = 0.f;
	attack_cd = 0.0;
	shield_cd = 0;
	flare_cd = 0;
	up = false;
	run_speed = 6.0;
	jump_speed = 7.0;
	key_no = 0;
	ground_height = 1600.f;

	step_max = player_run_stepMax;

	

	//AB = coll->Abinit({ vertices[2].position.x*m_scale.x, vertices[2].position.y*m_scale.y }, { vertices[0].position.x*m_scale.x, vertices[0].position.y*m_scale.y }, player);
	
	vertex = { vec2{vertices[0].position.x*m_scale.x,vertices[0].position.y*m_scale.y}
	,vec2{vertices[1].position.x*m_scale.x,vertices[1].position.y*m_scale.y}
	,vec2{vertices[2].position.x*m_scale.x,vertices[2].position.y*m_scale.y}
	,vec2{vertices[3].position.x*m_scale.x,vertices[3].position.y*m_scale.y}};
	
	foot_point = { (vertices[1].position.x*m_scale.x - vertices[0].position.x*m_scale.x) / 2.f + vertices[0].position.x*m_scale.x,
		(vertices[1].position.y*m_scale.y - vertices[0].position.y*m_scale.y) / 2.f + vertices[0].position.y*m_scale.y };

	//coll->addverindices(AB, indicess);
	//coll->addvertex(AB, vertex);
		//setting box2d bodydef
	playerBodyDef.type = b2_dynamicBody;
	float32 x = m_position.x * pixeltob2;
	float32 y = m_position.y * pixeltob2;
	playerBodyDef.position.Set(x, y);
	playerBodyDef.angle = 0;
	//setting box2d shape and fixture def
	float32 width = abs(m_scale.x* player_texture.width * pixeltob2 * 0.005);
	float32 height = abs(m_scale.y* player_texture.height * pixeltob2 * 0.005);
	playerShape.SetAsBox(width/2,height/2);
	playerFixtureDef.shape = &playerShape;
	playerFixtureDef.density = 1;
	playerFixtureDef.friction = 0.3;

	return true;
}

void Player::reset(vec2 position, int exp, int lv) {
	m_is_alive = true;
	left = false;
	right = false;
	shielded = false;
	//shield_countDown = 0;
	m_position = position;
	m_rotation = 0.f;
	attack_cd = 0.0;
	shield_cd = 0;
	flare_cd = 0;
	start_time = 0.0;
	run = false;
	cast = false;
	lifePoint = 10;
	total_exp = exp;
	physic.init(true);
	up = false;
	level = lv;
	key_no = 0;
	jump_countDown = Global_jump_CountDown;
	items[0]->addDef();
	items[0]->rigidbody = b2world->CreateBody(&items[0]->sleepBodyDef);
	items[0]->rigidbody->CreateFixture(&items[0]->sleepFixtureDef);
	items[0]->rigidbody->SetUserData(items[0]);
	can_use_item = false;
}

void Player::use_item(int index) {
	if (items.size() >= index && items[index - 1]->cd <= 0.f && can_use_item) {
		items[index-1]->init(m_position, physic.face_right ? vec2{1.f, 0.f} : vec2{-1.f, 0.f}, 4000.f, 5000.f);
	}
}

bool Player::is_item0_usable() {
	return can_use_item && items[0]->cd <= 0.f;
}
// Releases all graphics resources
void Player::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	for (auto& item : items) {
		delete item;
	}
	//
	effect.release();
}

void Player::set_Direction(int dir, bool value) {


	switch (dir)
	{
		case 1:
			if (value) physic.add_Force({ -run_speed, 0.f });
			else physic.delete_Force({ -run_speed, 0.f });
			left = value;
			break;
		case 2:
			if (value) physic.add_Force({ run_speed, 0.f });
			else physic.delete_Force({ run_speed, 0.f });
			right = value;
			break;
		case 3:
			if (value && physic.ground) physic.jump_force = vec2({ 0.f, -jump_speed });
			up = value;
			break;
		default:
			break;
	}
	if (left || right) {           // if the player is moving, set run mod true
		if (!run) tex_num = 0;        //Reset the tex number when changing animation to run
		run = true;
	}
	else                 // if the player stop moving, set run mod false;
	{
		if (run) tex_num = 0;        //Reset the tex number when changing animation to stand
		run = false;
	}
}


// Called on each frame by World::update()
void Player::update(float ms)
{
	if (attack_cd > 0) attack_cd -= ms;
	if (shield_cd > 0) shield_cd -= ms;
	if (flare_cd > 0) flare_cd -= ms;
	start_time += ms;

	if (m_position.y >= 1800.f) kill();

	if (swamp_sound_countdown != 0)
		swamp_sound_countdown--;

	/*if (shield_countDown == 0)
		shielded = false;
	else
		shield_countDown--;*/
	if (!m_is_alive) physic.forces.clear();
	vec2 motion = physic.cal_Force(General_friction);
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// UPDATE player POSITION HERE BASED ON KEY PRESSED (World::on_key())
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	for (auto& item : items) {
		if (item->inited || item->cd > 0.f) item->update(ms);
	}

	if (abs(motion.x) > step_max && physic.orb_jump <= 0.f) motion.x = motion.x / abs(motion.x) * step_max;  // horizontal motion can't exceed max s
	if (abs(motion.y) > player_jump_stepMax) motion.y = motion.y / abs(motion.y) * player_jump_stepMax;  // horizontal motion can't exceed max s
	if (m_position.y >= ground_height) {          // check in the air
		physic.support_force = vec2({ 0.f, -(abs(physic.motion.y) + Global_gravity) });
		m_position.y = ground_height;
		physic.ground = true;
		onCube = false;
	}
	else {
		if (onCube == true) {
			physic.support_force = vec2({ 0.f, -(abs(physic.motion.y) + Global_gravity) });
			physic.ground = true;
		}
		else {
			physic.support_force = vec2({ 0.f, 0.f });
			physic.ground = false;
		}
	}

	move(vec2{ motion.x * ms / 10.f, motion.y * ms / 20.f });

	if (start_time >= Gloabal_animation_CountDown) {
		tex_num = (tex_num + 1) % 10;
		if (!m_is_alive) {
			player_texture = die_textures[tex_num];
		}
		else if (cast) {
			player_texture = cast_textures[tex_num];
		}
		else if (motion.y < 0.0) {      // the player animation shifts to jump if he is in the air
			player_texture = jump_textures[tex_num];
		}
		else if (motion.y > 0.0) {      // the player animation shifts to down if he reaches the highest jump height
			player_texture = down_textures[tex_num];
		}
		else if (run) {                                                // if it runs set the path to run
			player_texture = run_textures[tex_num];
		}

		else
		{
			player_texture = stand_textures[tex_num];
		}
		start_time = 0.0;
		if (cast && tex_num == 9) cast = false;
		if (!m_is_alive) {
			if (tex_num == 9) start_time = -99999.f;      // if the animation of dying finishes, make the frame_countDown plus one so that it won't play the dying again.
		}
	}

	if (motion.x != 0 && swampContacting) {
		if (swamp_sound_countdown == 0) {
			if (swamp_sound_switch == false)
			{
				Mix_PlayChannel(-1, m_swamp, 0);
				swamp_sound_countdown = 100;
				swamp_sound_switch = true;
			}
			else {
				Mix_PlayChannel(-1, m_swamp2, 0);
				swamp_sound_countdown = 100;
				swamp_sound_switch = false;
			}
		}
	}
	foot_point = { m_position.x, m_position.y + ((vertex[2].y - vertex[0].y) / 2.f) };

	healthp->set_position({ get_position().x, get_position().y - 150 });
	b2Vec2 b2position = b2Vec2((float32)(m_position.x * pixeltob2), (float32)(m_position.y* pixeltob2));
	if (rigidbody) { rigidbody->SetTransform(b2position, 0); }

	if (countdown > 0) { countdown--; }
	if (enemyContacting > 0 && countdown <= 0) { checkEnemy(); }
	if (fireContacting && countdown <= 0) { checkFire(); }
	//if (venomContacting && countdown <= 0) { checkVenom(); }
	if (swampContacting) {

		checkSwamp();
	}
}

void Player::draw(const mat3& projection, mat3& view_2d)
{
	transform_begin();

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// player TRANSFORMATION CODE HERE

	// see Transformations and Rendering in the specification pdf
	// the following functions are available:
	// transform_translate()
	// transform_rotate()
	// transform_scale()

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// REMOVE THE FOLLOWING LINES BEFORE ADDING ANY TRANSFORMATION CODE
	transform_translate(m_position);
	transform_rotate(m_rotation);
	transform_scale(m_scale);
	if (!physic.face_right) transform_mirror();
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	transform_end();
	/*
	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Getting uniform locations
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint light_up_uloc = glGetUniformLocation(effect.program, "light_up");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_color_loc = glGetAttribLocation(effect.program, "in_color");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_color_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);

	// !!! Player Color
	float color[] = { 1.f, 1.f, 1.f };
	float dead[] = {10.f, 0.f, 0.f};
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	if (!m_is_alive) {
		glUniform3fv(color_uloc, 1, dead);
		glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HERE TO SET THE CORRECTLY LIGHT UP THE player IF HE HAS EATEN RECENTLY
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	int light_up = 0;
	if (m_light_up_countdown_ms > 0) light_up = 1;
	glUniform1iv(light_up_uloc, 1, &light_up);
	*/
	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint view_uloc = glGetUniformLocation(effect.program, "view");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, player_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);


	// Drawing!
	glDrawElements(GL_TRIANGLES,6, GL_UNSIGNED_SHORT, nullptr);
	for (auto& item : items) {
		if (item->inited)
			item->draw(projection, view_2d);
	}
}

// Simple bounding box collision check,
bool Player::collides_with(const Enemy& enemy)
{
	if (!shielded && m_is_alive) {
		attacked(1);		
		return true;
	}
	return false;
}

// Simple bounding box collision check,
bool Player::collides_with(const Fire& fire)
{
	if (!shielded && m_is_alive) {
		attacked(1);
		return true;
	}
	return false;
}

bool Player::collides_with(const Fireball& fire)
{
	if (!shielded && m_is_alive) {

		attacked(1);
		return true;
	}
	return false;
}

int Player::get_keys() const {
	return key_no;
}

void Player::add_keys() {
	key_no++;
}

void Player::delete_keys() {
	key_no--;
}

void Player::set_keys(int no) {
	key_no = no;
}

bool Player::collides_with(const Ground& ground)
{
	float ground_v0_x = ground.vertex[0].x + ground.get_position().x;  // min
	float ground_v2_x = ground.vertex[2].x + ground.get_position().x; // max
	float ground_v0_y = ground.vertex[0].y + ground.get_position().y;  //min
	float ground_v2_y = ground.vertex[2].y + ground.get_position().y; // max

	
	if (foot_point.x >= ground_v0_x && foot_point.x <= ground_v2_x && foot_point.y <= ground_v0_y && foot_point.y >= ground_v2_y) {
		
		return true;
	}
	else {
		return false;
	}

}


bool Player::collides_with(const Mace & mace)
{
	if (m_is_alive) {
		attacked(3);
		return true;
	}
	return false;
}

bool Player::collides_with(const Venom & venom)
{
	if (m_is_alive) {
		attacked(1);
		return true;
	}
	return false; 
}

/*
bool Player::collides_with(Cross & cross)
{
	if (m_is_alive) {
		if (lifePoint < 10 && lifePoint > 0 && !cross.used)
		{
			if (lifePoint < 10)
				lifePoint = lifePoint + 5;
			if (lifePoint > 10)
				lifePoint = 10;
			healthp->current_health_level = lifePoint;
			cross.used = true;
		}
	}
}*/

/*
bool Player::collides_with(Stone & stonee)
{
	return false;
}*/

/*
bool Player::collides_with(const Fire_e & df)
{
	return false;
}*/



vec2 Player::get_position()const
{
	return m_position;
}

//int Player::get_texture_height() {
//	return player_texture.height;
//}
//
//vec2 Player::get_m_scale(){
//	return m_scale;
//}

bool Player::check_near_npc() {
	return (m_position.x <= 2077.f && m_position.x >= 1877.f && m_position.y <= 260.f && m_position.y >= 200.f);
}

void Player::move(vec2 off)
{
	m_position.x += off.x; m_position.y += off.y;
}

void Player::set_rotation(float radians)
{
	m_rotation = radians;
}

bool Player::is_alive()const
{
	return m_is_alive;
}

// Called when the player collides with a enemy
void Player::kill()
{
	m_is_alive = false;
}



Attack_P* Player::attack(bool att) {
	Attack_P *attack = new Attack_P;
	if (att && attack_cd <= 0.0) {
		attack->created = true;
		if (physic.face_right) {
			attack->init(m_position, { 1, 0 });
		}
		else
		{
			attack->init(m_position, { -1, 0 });
		}
		Mix_PlayChannel(-1, m_player_attack_sound, 0);
		attack_cd = player_normal_attack_cd;
		cast = att;
	}
	else {
		delete attack;
		attack = NULL;
	}

	if (!att) { 
		cast = att;
	}
	return attack;
}


Flare_P* Player::cast_flare(bool flare) {
	Flare_P* f = new Flare_P;
	if (flare && flare_cd <= 0.0 && level >= 5) {
		f->created = true;
		Mix_PlayChannel(-1, m_player_explosion_sound, 0);
		if (physic.face_right) {
			f->init(m_position, { 1, 0 });
		}
		else
		{
			f->init(m_position, { -1, 0 });
		}
		flare_cd = player_normal_flare_cd;
		cast = flare;
	}
	else {
		delete f;
		f = NULL;
	}

	if (!flare) { 
		cast = flare;
	}
	return f;
}

//Shield_P* Player::cast_shield(bool shield) {
//	Shield_P* newshield = new Shield_P;
//	if (shield && shield_cd <= 0.0 && level >= 2) {
//		if (physic.face_right) {
//			newshield->init(m_position, { 1, 0 }, this);
//		}
//		else
//		{
//			newshield->init(m_position, { -1, 0 }, this);
//		}
//		Mix_PlayChannel(-1, m_player_shield_sound, 0);
//		shield_cd = player_normal_shield_cd;
//		cast = shield;
//	}
//	if (!shield) cast = shield;
//	return newshield;
//}

Shield_P* Player::cast_shield(bool shield) {
	Shield_P* newshield = new Shield_P;
	if (shield && shield_cd <= 0.0 && level >= 2) {
		newshield->created = true;
		if (physic.face_right) {
			newshield->init(m_position, { 1, 0 }, this);
		}
		else
		{
			newshield->init(m_position, { -1, 0 }, this);
		}
		Mix_PlayChannel(-1, m_player_shield_sound, 0);
		shield_cd = player_normal_shield_cd;
		cast = shield;
	}
	else {
		delete newshield;
		newshield = NULL;
	}
	if (!shield) { 
		cast = shield;
	}
	return newshield;
}


bool Player::deadOrNot() {
	if (lifePoint == 0)
	    return true;
	else
		return false;
}

void Player::reset_texnum() {
	tex_num = 0;
}

bool Player::get_casting() const {
	return cast;
}

sPlayer* Player::getInfo()
{
	sPlayer* s1 = new  sPlayer;
	//s1->healthp = healthp;
	s1->attack_cd = attack_cd;
	s1->cast = cast;
	s1->flare_cd = flare_cd;
	s1->foot_point = foot_point;
	s1->jump_countDown = jump_countDown;
	s1->jump_speed = jump_speed;
	s1->key_no = key_no;
	s1->last_position = last_position;
	s1->left = left;
	s1->level = level;
	s1->lifePoint = lifePoint;
	s1->m_is_alive = m_is_alive;
	s1->m_position = m_position;
	s1->m_rotation = m_rotation;
	s1->m_scale = m_scale;
	s1->needcheck = needcheck;
	s1->onCube = onCube;
	s1->right = right;
	s1->run = run;
	s1->run_speed = run_speed;
	s1->shielded = shielded;
	s1->shield_cd = shield_cd;
	s1->start_time = start_time;
	s1->tex_num = tex_num;
	s1->total_exp = total_exp;
	s1->up = up;
	s1->can_use_item = can_use_item;
	return s1;
}

void Player::setInfo(sPlayer* p)
{

	attack_cd = p->attack_cd;
	//healthp = p->healthp;
	cast = p->cast;
	flare_cd = p->flare_cd;
	foot_point = p->foot_point;
	jump_countDown = p->jump_countDown;
	jump_speed = p->jump_speed;
	key_no = p->key_no;
	last_position = p->last_position;
	left = p->left;
	level = p->level;
	lifePoint = p->lifePoint;
	m_is_alive = p->m_is_alive;
	m_position = p->m_position;
	m_rotation = p->m_rotation;
	m_scale = p->m_scale;
	needcheck = p->needcheck;
	onCube = p->onCube;
	right = p->right;
	run = p->run;
	run_speed = p->run_speed;
	shielded = p->shielded;
	shield_cd = p->shield_cd;
	start_time = p->start_time;
	tex_num = p->tex_num;
	total_exp = p->total_exp;
	up = p->up;
	can_use_item = p->can_use_item;
	if(healthp!= NULL){ 
		healthp->current_health_level = lifePoint;
	}
	else {
	    
	}

}

void Player::set_shield(bool shield) {
	shielded = shield;
	/*shield_countDown = 150;*/
}

void Player::setHPbar(HealthPoints *hp) {
	this->healthp = hp;
}

int Player::get_exp() const {
	return total_exp;
}

int Player::get_level() const {
	return level;
}

void Player::add_exp(int e) {
	total_exp += e;
}

int Player::get_hp() const{
	return lifePoint;
}

void Player::set_height_limit(float h) {
	ground_height = h;
	OriginalHeight = h;
}

int Player::getEntityType()
{
	return ET_player;
}

void Player::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
	//if (r2EntityType == ET_goldball) {
	
	if (r2EntityType == ET_player_flare) {
	
		
	}
	else if (r2EntityType == ET_ground) {

		collide_Ground(*((Ground*)r2));
	}
	else {
		r2->handleCollision(r2, r1, r2EntityType, r1EntityType);
	}
}

void Player::checkEnemy()
{


		for (b2ContactEdge* ce = rigidbody->GetContactList(); ce; ce = ce->next)
		{

			void* bodyUserData = ce->other->GetUserData();
			if (bodyUserData) {
				int Bentitytype = static_cast<Renderable*>(bodyUserData)->getEntityType();

				if (((Bentitytype % 100) / 10) == 1 && ce->contact->IsTouching())
				{
					if (!shielded) { attacked(1); }
					countdown = 70;
					return;
				}
			}

			if (ce->next == NULL) { enemyContacting = 0; }

		}

		enemyContacting = 0;
	
}

void Player::checkFire()
{

	for (b2ContactEdge* ce = rigidbody->GetContactList(); ce; ce = ce->next)
	{

		void* bodyUserData = ce->other->GetUserData();
		if (bodyUserData) {
			int Bentitytype = static_cast<Renderable*>(bodyUserData)->getEntityType();

			if ( Bentitytype == ET_fire && ce->contact->IsTouching())
			{
				if (!shielded) { attacked(1); }
				countdown = 70;
				return;
			}
		}

		if (ce->next == NULL) { fireContacting = false; }

	}

	fireContacting = false;
}

void Player::checkVenom()
{
	for (b2ContactEdge* ce = rigidbody->GetContactList(); ce; ce = ce->next)
	{

		void* bodyUserData = ce->other->GetUserData();
		if (bodyUserData) {

			int Bentitytype = static_cast<Renderable*>(bodyUserData)->getEntityType();

			if (Bentitytype == ET_venom && ce->contact->IsTouching())
			{

				Renderable* venom = static_cast<Renderable*>(bodyUserData);

				venom->handleCollision(venom, this, ET_venom, ET_player);
				
				if (venomContacting) {
					attacked(1);
				    countdown = 70;
				}

				venomContacting = true;
				return;
			}
		}

		if (ce->next == NULL) { venomContacting = false; }

	}

	venomContacting = false;

}

void Player::checkSwamp()
{
	for (b2ContactEdge* ce = rigidbody->GetContactList(); ce; ce = ce->next)
	{

		void* bodyUserData = ce->other->GetUserData();
		if (bodyUserData) {
			int Bentitytype = static_cast<Renderable*>(bodyUserData)->getEntityType();



			if (Bentitytype == ET_swamp && ce->contact->IsTouching())
			{
				



				if (step_max >= player_run_stepMax) { 
					
					step_max = step_max / 2 ; };

				return;
			}
		}

		if (ce->next == NULL) {

			/*
			if ( 6 > run_speed >= 3) { run_speed = run_speed * 2; };
			swampContacting = false; }*/

		}
	}

	if (step_max < player_run_stepMax) {

			step_max = player_run_stepMax;
	};
	swampContacting = false;
}

void Player::collide_Ground(Ground & ground)
{
	//onCube = true;
	groundContacting ++;
	ground_height = ground.Gstartpoint.y * b2topixel;

}

void Player::checkGround()
{
	for (b2ContactEdge* ce = rigidbody->GetContactList(); ce; ce = ce->next)
	{

		void* bodyUserData = ce->other->GetUserData();
		if (bodyUserData) {
			int Bentitytype = static_cast<Renderable*>(bodyUserData)->getEntityType();

			if (Bentitytype == ET_ground && ce->contact->IsTouching())
			{

				float distance = m_position.y - (static_cast<Ground*>(bodyUserData))->get_position().y;
				if (10 < distance < 40) {
				
					m_position.y += 100;
					physic.motion.y = 20;
					return;
				}


				b2WorldManifold worldManifold;
				ce->contact->GetWorldManifold(&worldManifold);

				int fixture1 = (static_cast<Renderable*> (ce->contact->GetFixtureA()->GetBody()->GetUserData()))->getEntityType();
				int fixture2 = (static_cast<Renderable*> (ce->contact->GetFixtureB()->GetBody()->GetUserData()))->getEntityType();

				b2Vec2 normal = worldManifold.normal;  // poiting from fixture 1 to fixture 2

				printf("%4.2f %4.2f \n", normal.x, normal.y);

				if (fixture1 == ET_ground) {

					if (normal.y == 1) {
						physic.ground = true;
						ground_height = (static_cast<Ground*>(bodyUserData))->Gstartpoint.y * b2topixel;

						if (onCube == false) {
							physic.motion.y = 0;
							onCube = true;
						}

					}
					else if (normal.y == -1) {

						printf("from bottom up");
						physic.motion.y = 1;
						m_position.y = (static_cast<Ground*>(bodyUserData))->Gstartpoint.y * b2topixel + 50;


					}
					else {

						printf("from side");

					}
				}
				else {
				
					if (normal.y == -1) {
						physic.ground = true;
						ground_height = (static_cast<Ground*>(bodyUserData))->Gstartpoint.y * b2topixel;

						if (onCube == false) {
							physic.motion.y = 0;
							onCube = true;
						}



					}
					else if (normal.y == 1) {

						printf("from bottom up");
						physic.motion.y = 1;

					    m_position.y = (static_cast<Ground*>(bodyUserData))->Gstartpoint.y * b2topixel + 50;

					}
					else {

						printf("from side");
					}
						
				}
			

				return;
			}
		}

		if (ce->next == NULL) {

			/*
			if ( 6 > run_speed >= 3) { run_speed = run_speed * 2; };
			swampContacting = false; }*/

		}
	}


	ground_height = OriginalHeight;
	onCube = false;
	//physic.ground = false;

	groundContacting = 0;
}

vec2 Player::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * player_texture.width * 0.008f, std::fabs(m_scale.y) *player_texture.height * 0.008f };
}

Level_Up Player::level_u() {
	Level_Up lv;
	if (total_exp > 20 && level < 2) {
		level++;
		Mix_PlayChannel(-1, m_player_lvlup_sound, 0);
		if (physic.face_right) {
			lv.init(m_position, { 1, 0 }, this);
		}
		else
		{
			lv.init(m_position, { -1, 0 }, this);
		}
	}
	if (total_exp > 50 && level < 3) {
		level++;
		Mix_PlayChannel(-1, m_player_lvlup_sound, 0);

		if (physic.face_right) {
			lv.init(m_position, { 1, 0 }, this);
		}
		else
		{
			lv.init(m_position, { -1, 0 }, this);
		}
	}
	if (total_exp > 90 && level < 4) {
		level++;
		Mix_PlayChannel(-1, m_player_lvlup_sound, 0);

		if (physic.face_right) {
			lv.init(m_position, { 1, 0 }, this);
		}
		else
		{
			lv.init(m_position, { -1, 0 }, this);
		}
	}
	if (total_exp > 150 && level < 5) {
		level++;
		Mix_PlayChannel(-1, m_player_lvlup_sound, 0);
		if (physic.face_right) {
			lv.init(m_position, { 1, 0 }, this);
		}
		else
		{
			lv.init(m_position, { -1, 0 }, this);
		}
	}
	if (total_exp > 270 && level < 6) {
		level++;
		Mix_PlayChannel(-1, m_player_lvlup_sound, 0);
		if (physic.face_right) {
			lv.init(m_position, { 1, 0 }, this);
		}
		else
		{
			lv.init(m_position, { -1, 0 }, this);
		}
	}
	if (total_exp > 1270 && level < 7) {
		level++;
		Mix_PlayChannel(-1, m_player_lvlup_sound, 0);
		if (physic.face_right) {
			lv.init(m_position, { 1, 0 }, this);
		}
		else
		{
			lv.init(m_position, { -1, 0 }, this);
		}
	}
	return lv;
}

void Player::attacked(int h) {
	if (lifePoint >= h) { 
		lifePoint -= h;
	}
	else {
		lifePoint = 0;
	}
	healthp->current_health_level = lifePoint;
	if (deadOrNot()) {
		kill();
	}
}