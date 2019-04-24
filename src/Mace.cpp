#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "mace.hpp"

Texture Mace::mace_textures[10];

bool Mace::init(vec2 position) {
	char texPath[] = ground_mace;
	char texName[] = "0.png";
	char buffer[256];
	strncpy(buffer, texPath, sizeof(buffer));
	strncat(buffer, texName, sizeof(buffer));
	mace_texture.load_from_file(buffer);

	float wr = mace_texture.width;
	float hr = mace_texture.height;

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
	m_scale.x = 0.25f;
	m_scale.y = 0.25f;

	vertex = { vec2{vertices[0].position.x*m_scale.x,vertices[0].position.y*m_scale.y}
,vec2{vertices[1].position.x*m_scale.x,vertices[1].position.y*m_scale.y}
,vec2{vertices[2].position.x*m_scale.x,vertices[2].position.y*m_scale.y}
,vec2{vertices[3].position.x*m_scale.x,vertices[3].position.y*m_scale.y} };

	return true;
}

void Mace::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	effect.release();
}

void Mace::draw(const mat3& projection, mat3& view_2d)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(m_position);
	transform_translate({ 0,300.f});
	transform_rotate(m_rotation);
	transform_translate({ 0,0-300.f});
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
	glBindTexture(GL_TEXTURE_2D, mace_textures[0].id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Mace::get_position()const
{
	return m_position;
}

void Mace::update(float ms) {
	m_rotation += ms / 1300;
	center.x = m_position.x;
	center.y = m_position.y + 300;
	
	head_a_position.x = (head_a_original_position.x - center.x) * cos(m_rotation) - (head_a_original_position.y - center.y) * sin(m_rotation);
	head_a_position.y = (head_a_original_position.x - center.x) * sin(m_rotation) + (head_a_original_position.y - center.y) * cos(m_rotation);
	head_b_position.x = (head_b_original_position.x - center.x) * cos(m_rotation) - (head_b_original_position.y - center.y) * sin(m_rotation);
	head_b_position.y = (head_b_original_position.x - center.x) * sin(m_rotation) + (head_b_original_position.y - center.y) * cos(m_rotation);

	head_a_position.x += center.x;
	head_a_position.y += center.y;
	head_b_position.x += center.x;
	head_b_position.y += center.y;

	if (rigidbody) {
	
		rigidbody->SetTransform(b2Vec2(pixeltob2 * (head_a_position.x + head_b_position.x)/2, pixeltob2 * (head_a_position.y+head_b_position.y)/2),0);
	}

}

bool Mace::collide_with(Player & player)
{
	if (player.countdown <= 0) {
		player.attacked(3);
		player.countdown = 70;
	}
	return true;
}

vec2 Mace::get_head_a_position() {
	return head_a_position;
}

vec2 Mace::get_head_b_position() {
	return head_b_position;
}

vec2 Mace::get_head_c_position() {
	return head_c_position;
}

vec2 Mace::get_head_d_position() {
	return head_d_position;
}

int Mace::getEntityType()
{
	return ET_mace;
}

void Mace::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
	if (r2EntityType == ET_player) {
	
		collide_with(*(Player*)r2);
	}
}

void Mace::setDef()
{
	//setting box2d bodydef
	maceBodyDef.type = b2_kinematicBody;
	float32 x = m_position.x * pixeltob2;
	float32 y = m_position.y * pixeltob2;
	maceBodyDef.position.Set(x, y);
	maceBodyDef.angle = 0;
	//setting box2d shape and fixture def

	float32 radius =  0.025 * ((mace_texture.width * m_scale.x + mace_texture.height * m_scale.y)* pixeltob2) / 4 ;
	maceShape.m_radius = radius;
	maceFixtureDef.shape = &maceShape;
    maceFixtureDef.density = 0.5;
	maceFixtureDef.friction = 0.3;
}
