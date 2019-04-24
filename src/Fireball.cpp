// stlib
#include <vector>
#include <string>
#include <algorithm>
#include "Fireball.hpp"

Texture Fireball::fireball_textures[Ground_fireball_textures];

bool Fireball::init(vec2 position, vec2 direction)
{
	char texPath[] = ground_fireball;
	char texName[] = "0.png";
	char buffer[256];
	strncpy(buffer, texPath, sizeof(buffer));
	strncat(buffer, texName, sizeof(buffer));
	texture.load_from_file(buffer);
	frametime_prev = 0.0;
	duration_countdown = 50;
	start_time = 0.0;
	tex_num = 0;

	float wr = texture.width;
	float hr = texture.height;

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
	//m_scale.x = -35.f;
	//m_scale.y = 35.f;
	m_position = position;
	m_direction = direction;
	m_scale.x = 0.5f;
	m_scale.y = 0.5f;


	vertex = { vec2{vertices[0].position.x*m_scale.x,vertices[0].position.y*m_scale.y}
	,vec2{vertices[1].position.x*m_scale.x,vertices[1].position.y*m_scale.y}
	,vec2{vertices[2].position.x*m_scale.x,vertices[2].position.y*m_scale.y}
	,vec2{vertices[3].position.x*m_scale.x,vertices[3].position.y*m_scale.y} };

	return true;
}

void Fireball::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	effect.release();
}

void Fireball::updatePos() {
	// if reaches upper limit
	if (m_position.y < upper_limit && goup) {
		godown = true;
		goup = false;
	}

	//call everytime when ball reaches bottom
	if (m_position.y >= bottom && godown) {
		goup = true;
		godown = false;
		damping = (damping > 0) ? damping - 1 : 50;
		upper_limit = (upper_limit <= bottom) ? bottom - (500 * exp(-counter)) : bottom;
		counter = counter + 0.1;
	}
	
	if (counter > 2.4) {
		goup = false;
		godown = false;
		isOn = false;
		reset();
	}

	if (goup) {
		double ratio = (((bottom - m_position.y) / (bottom - upper_limit)));
		m_position.y = m_position.y - (50 * exp(-ratio * 1.2));
		//counter_up = counter_up + 0.0012;
		//counter_down = 3;
	}

	if (godown) {
		double ratio = ((bottom - m_position.y) / (bottom - upper_limit));
		m_position.y = m_position.y + (50 * exp(-ratio * 2.5));
		//counter_down = counter_down - 0.1;
		//counter_up = 0;
	}
	// move
	m_position.x = m_position.x + (10 * exp(-counter));
}

void Fireball::update(float ms)
{
	if (!isOn) return;
	start_time += ms;
	if (start_time >= 20) {
		tex_num = (tex_num + 1) % Ground_fireball_textures;
		duration_countdown--;
		start_time = 0.0;
		updatePos();
	}
	if (rigidbody)
	{
		b2Vec2 b2position = b2Vec2((float32)(m_position.x * pixeltob2), (float32)(m_position.y* pixeltob2));
		rigidbody->SetTransform(b2position, 0); 
	}
}

void Fireball::draw(const mat3& projection, mat3& view_2d)
{
	if (!isOn) return;
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
	glBindTexture(GL_TEXTURE_2D, fireball_textures[tex_num].id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Fireball::get_position()const
{
	return m_position;
}

void Fireball::set_position(vec2 position)
{
	m_position = position;
}

// Returns the local bounding coordinates scaled by the current size of the enemy 
vec2 Fireball::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * 0.4f*texture.width, std::fabs(m_scale.y) * 0.4f*texture.height };
}

bool Fireball::collides_with(Enemy& enemy)
{
	/*
	float dx = m_position.x - enemy.get_position().x;
	float dy = m_position.y - enemy.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(enemy.get_bounding_box().x - 50.f, enemy.get_bounding_box().y - 50.f);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 1.5f;
	if (d_sq < r * r) {
		return true;
	}
	return false;
	*/
	return true;
}

int Fireball::get_duration() {
	return duration_countdown;
}

void Fireball::show_fireball(bool status) {
	isOn = status;
}

void Fireball::reset() {
	damping = 20, upper_limit = 1400;
	counter = 0.0;
	goup = true;
	godown = false;
	isOn = false;
}

bool Fireball::fireball_status() {
	return isOn;
}

bool Fireball::collides_with(Player & player)
{
	if (fireball_status() &&!player.shielded && player.m_is_alive && player.countdown <= 0) {
		player.attacked(1);
		player.countdown = 70;
		return true;
	}
	return false;
}

void Fireball::setDef()
{
	//setting box2d bodydef
	fireBallBodyDef.type = b2_dynamicBody;
	float32 x = m_position.x * pixeltob2;
	float32 y = m_position.y * pixeltob2;
	fireBallBodyDef.position.Set(x, y);
	fireBallBodyDef.angle = 0;
	//setting box2d shape and fixture def

	float32 radius = ((texture.width + texture.height)*pixeltob2) *0.2 / 2;
	fireBallShape.m_radius = radius;
	fireBallFixtureDef.shape = &fireBallShape;
	fireBallFixtureDef.density = 1;
	fireBallFixtureDef.friction = 0.3;

}

int Fireball::getEntityType()
{
	return ET_fire_ball;
}

void Fireball::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
	if (r2EntityType == ET_player) {
		collides_with(*(Player*)r2);	
	}
	else if (r2EntityType == ET_ground) {
	
		////do stuff with ground

	}
}
