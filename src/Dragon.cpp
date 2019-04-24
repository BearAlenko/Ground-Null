// Header
#include "Dragon.hpp"
#include <vector>
#include <cmath>
#include <time.h>

Texture Dragon::idle_Textures[];
Texture Dragon::attack_Textures[];
Texture Dragon::fly_Textures[];
Texture Dragon::sleep_Texture;
Mix_Chunk* Dragon::m_dragon_fire;
Mix_Chunk* Thunder_e::m_lighting;
int Fire_e::cd_count;
int Thunder_e::cd_count;
bool Dragon::newInit(Player* player, std::vector<Fire>* m_fire, HealthPoints* heathp) {


	init(dragon_idle, player, m_fire, heathp);
	exp = 2000;
	hp = 50;
	boss = true;
	speed_add = 0;
	type = 4;
	m_scale = { 1.f, 1.f };
	texture = idle_Textures[0];
	fly = false;
	dragon = true;


	//setting box2d bodydef
	dragonBodyDef.type = b2_dynamicBody;
	float32 x = m_position.x * pixeltob2;
	float32 y = m_position.y * pixeltob2;
	dragonBodyDef.position.Set(x, y);
	dragonBodyDef.angle = 0;
	//setting box2d shape and fixture def
	float32 width = abs(m_scale.x* enemy_texture.width * pixeltob2 * 0.9);
	float32 height = abs(m_scale.y* enemy_texture.height * pixeltob2);
	dragonShape.SetAsBox(width / 2, height / 2);
	dragonFixtureDef.shape = &dragonShape;
	dragonFixtureDef.density = 1;
	dragonFixtureDef.friction = 0.3;

	return true;

}

void Dragon::set_sleep(float sleeptime) {
	sleep_time = sleeptime;
}

void Dragon::draw(const mat3& projection, mat3& view_2d)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(m_position);
	transform_rotate(m_rotation);
	transform_scale(m_scale);
	if (!physic.face_right) transform_mirror();
	transform_end();

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
	GLint distort_uloc = glGetUniformLocation(effect.program, "distort");

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
	Fire_e::cd_count = tex_num;
	Thunder_e::cd_count = run_tex;
	glActiveTexture(GL_TEXTURE0);
	if(!fly && sleep_time > 0.f) glBindTexture(GL_TEXTURE_2D, sleep_Texture.id);
	else if (attack&&!fly)
	{
		glBindTexture(GL_TEXTURE_2D, attack_Textures[tex_num].id);

	}
	else if (fly) {      // the enemy animation shifts to jump if he is in the air
		glBindTexture(GL_TEXTURE_2D, fly_Textures[run_tex].id);
	}
	else if (run)
		glBindTexture(GL_TEXTURE_2D, idle_Textures[run_tex].id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	//float color[] = { 1.f, 1.f, 1.f };
	float color[] = { rageColor.x, rageColor.y, rageColor.z };
	glUniform3fv(color_uloc, 1, color);
	glUniform1f(distort_uloc, distort);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Dragon::attack_player(bool att) {
	if (animation_done&& sleep_time <= 0.f) {
		int ac = 10;
		if (dragon) ac = 33;
		attack = att;
		action_count = 0;
	}
	if (dragon_sound_countdown == 0) {
		Mix_PlayChannel(-1, m_dragon_fire, 0);
		dragon_sound_countdown = 120;
	}

}

int Dragon::getEntityType()
{
	return ET_dragon;
}

void Dragon::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{

	if (r2EntityType == ET_sleep_position) {

		r2->handleCollision(r2, r1, r2EntityType, r1EntityType);

	}else if (r2EntityType == ET_player) {
		((Player*)r2)->enemyContacting++;
	}
	else if (r2EntityType == ET_player_attack || r2EntityType == ET_player_shield || r2EntityType == ET_player_flare) {


		r2->handleCollision(r2, r1, r2EntityType, r1EntityType);
	}
	/*
	else if (r2EntityType == ET_ground) {

		ground_height = ((Ground*)r2)->Gstartpoint.y * b2topixel;
		groundContacting++;
	}*/
}

Texture Fire_e::attack_e_textures[9];
bool Fire_e::init(vec2 position, vec2 direction)
{
	char texPath[] = fire_e;
	char texName[] = "0.png";
	char buffer[256];
	strncpy(buffer, texPath, sizeof(buffer));
	strncat(buffer, texName, sizeof(buffer));
	skill_texture.load_from_file(buffer);

	damage = 4;
	frametime_prev = 0.0;
	start_time = 0.0;
	tex_num = 0;

	float wr = skill_texture.width*0.016f;
	float hr = skill_texture.height*0.016f;

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

	//AABB* ab = coll.init({vertices[2].position.x, vertices[2].position.y}, { vertices[0].position.x, vertices[0].position.y }, attack_p);



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
	//m_scale.x = -35.f;
	//m_scale.y = 35.f;
	m_position = position;
	m_direction = direction;
	m_scale.x = 70.f;
	if (direction.x > 0) m_scale.x = -m_scale.x;
	m_scale.y = 70.f;


	vertex = { vec2{vertices[0].position.x*m_scale.x,vertices[0].position.y*m_scale.y}
,vec2{vertices[1].position.x*m_scale.x,vertices[1].position.y*m_scale.y}
,vec2{vertices[2].position.x*m_scale.x,vertices[2].position.y*m_scale.y}
,vec2{vertices[3].position.x*m_scale.x,vertices[3].position.y*m_scale.y} };

	return true;
}

void Fire_e::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	effect.release();
}

void Fire_e::update(float ms)
{
	if (tex_num >= 8) shouldErase = 1;
	start_time += ms;
	if (start_time >= Gloabal_animation_CountDown) {
		tex_num = (tex_num + 1) % 9;
		start_time = 0.0;
	}

}

void Fire_e::draw(const mat3& projection, mat3& view_2d)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(m_position);
	transform_scale(m_scale);
	transform_end();

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
	glBindTexture(GL_TEXTURE_2D, attack_e_textures[tex_num].id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Fire_e::get_position()const
{
	return m_position;
}

void Fire_e::set_position(vec2 position)
{
	m_position = position;
}

// Returns the local bounding coordinates scaled by the current size of the enemy 
vec2 Fire_e::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * 0.4f*skill_texture.width, std::fabs(m_scale.y) * 0.4f*skill_texture.height };
}

int Fire_e::getEntityType()
{
	return ET_dragon_fire;
}

void Fire_e::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
	if (r2EntityType == ET_player) {
	
		collides_with(*(Player*)r2);
	}
}

void Fire_e::setDef()
{
	//setting box2d bodydef
	dragonFireBodyDef.type = b2_dynamicBody;
	float32 x = m_position.x * pixeltob2;
	float32 y = m_position.y * pixeltob2;
	dragonFireBodyDef.position.Set(x, y);
	dragonFireBodyDef.angle = 0;
	//setting box2d shape and fixture def
	float32 width = abs(m_scale.x* skill_texture.width * pixeltob2 * 0.25);
	float32 height = abs(m_scale.y* skill_texture.height * pixeltob2 * 0.01);
	dragonFireShape.SetAsBox(width / 2, height / 2);
	dragonFireFixtureDef.shape = &dragonFireShape;
	dragonFireFixtureDef.density = 1;
	dragonFireFixtureDef.friction = 0.3;

}

bool Fire_e::collides_with(Player& player)
{   /*
	float dx = m_position.x - enemy.get_position().x;
	float dy = m_position.y - enemy.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(enemy.get_bounding_box().x - 30.f, enemy.get_bounding_box().y - 30.f);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 1.5f;
	if (d_sq < r * r) {*/
	if (player.countdown <= 0) {
		player.countdown = 70;
		player.attacked(damage);
	}
	return true;
	//	}
	//    return false;
}



Texture Thunder_e::thunder_e_textures[5];
bool Thunder_e::init(vec2 position, vec2 direction)
{
	char texPath[] = thunder_e;
	char texName[] = "0.png";
	char buffer[256];
	strncpy(buffer, texPath, sizeof(buffer));
	strncat(buffer, texName, sizeof(buffer));
	skill_texture.load_from_file(buffer);

	damage = 3;
	frametime_prev = 0.0;
	start_time = 0.0;
	tex_num = 0;

	float wr = skill_texture.width*0.016f;
	float hr = skill_texture.height*0.016f;

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

	//AABB* ab = coll.init({vertices[2].position.x, vertices[2].position.y}, { vertices[0].position.x, vertices[0].position.y }, attack_p);



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
	//m_scale.x = -35.f;
	//m_scale.y = 35.f;
	m_position = position;
	m_direction = direction;
	m_scale.x = 70.f;
	if (direction.x > 0) m_scale.x = -m_scale.x;
	m_scale.y = 70.f;


	vertex = { vec2{vertices[0].position.x*m_scale.x,vertices[0].position.y*m_scale.y}
,vec2{vertices[1].position.x*m_scale.x,vertices[1].position.y*m_scale.y}
,vec2{vertices[2].position.x*m_scale.x,vertices[2].position.y*m_scale.y}
,vec2{vertices[3].position.x*m_scale.x,vertices[3].position.y*m_scale.y} };

	return true;
}

void Thunder_e::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	effect.release();
}

void Thunder_e::update(float ms)
{
	if (tex_num >= 4) shouldErase = 1;
	start_time += ms;
	if (start_time >= Gloabal_animation_CountDown) {
		tex_num = (tex_num + 1) % 5;
		start_time = 0.0;
	}

}

void Thunder_e::draw(const mat3& projection, mat3& view_2d)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(m_position);
	transform_scale(m_scale);
	transform_end();

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
	glBindTexture(GL_TEXTURE_2D, thunder_e_textures[tex_num].id);
	Mix_PlayChannel(-1, m_lighting, 0);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Thunder_e::get_position()const
{
	return m_position;
}

void Thunder_e::set_position(vec2 position)
{
	m_position = position;
}

// Returns the local bounding coordinates scaled by the current size of the enemy 
vec2 Thunder_e::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * 0.4f*skill_texture.width, std::fabs(m_scale.y) * 0.4f*skill_texture.height };
}

int Thunder_e::getEntityType()
{
	return ET_dragon_thunder;
}

void Thunder_e::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
	if (r2EntityType == ET_player) {
	
		collides_with(*(Player*)r2);
	
	}
}

void Thunder_e::setDef()
{
	//setting box2d bodydef
	dragonThunderBodyDef.type = b2_dynamicBody;
	float32 x = m_position.x * pixeltob2;
	float32 y = m_position.y * pixeltob2;
	dragonThunderBodyDef.position.Set(x, y);
	dragonThunderBodyDef.angle = 0;
	//setting box2d shape and fixture def
	float32 width = abs(m_scale.x* skill_texture.width * pixeltob2 * 0.02);
	float32 height = abs(m_scale.y* skill_texture.height * pixeltob2 * 0.20);
	dragonThunderShape.SetAsBox(width / 2, height / 2);
	dragonThunderFixtureDef.shape = &dragonThunderShape;
	dragonThunderFixtureDef.density = 1;
	dragonThunderFixtureDef.friction = 0.3;

}

bool Thunder_e::collides_with(Player& player)
{   /*
	float dx = m_position.x - enemy.get_position().x;
	float dy = m_position.y - enemy.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(enemy.get_bounding_box().x - 30.f, enemy.get_bounding_box().y - 30.f);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 1.5f;
	if (d_sq < r * r) {*/
	if (player.countdown <= 0) {
		player.countdown = 70;
		player.attacked(damage);
	}
	return true;
	//	}
	//    return false;
}