#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "GoldenOrb.hpp"




Texture GoldenOrb::orb_textures[10];

bool GoldenOrb::init(vec2 position)
{
	char texPath[] = ground_goldenorb;
	char texName[] = "0.png";
	char buffer[256];
	strncpy(buffer, texPath, sizeof(buffer));
	strncat(buffer, texName, sizeof(buffer));
	orb_texture.load_from_file(buffer);

	start_time = 0.0;
	tex_num = 0;

	float wr = orb_texture.width;
	float hr = orb_texture.height;

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
	m_scale.x = 1.f;
	m_scale.y = 1.f;

	vertex = { vec2{vertices[0].position.x*m_scale.x,vertices[0].position.y*m_scale.y}
,vec2{vertices[1].position.x*m_scale.x,vertices[1].position.y*m_scale.y}
,vec2{vertices[2].position.x*m_scale.x,vertices[2].position.y*m_scale.y}
,vec2{vertices[3].position.x*m_scale.x,vertices[3].position.y*m_scale.y} };

	//setting box2d bodydef
	goldenBodyDef.type = b2_staticBody;
	float32 x = position.x * pixeltob2; 
	float32 y = position.y * pixeltob2;
	goldenBodyDef.position.Set(x, y);
	goldenBodyDef.angle = 0;

	//setting box2d shape and fixture def
	float32 width = abs(m_scale.x*orb_texture.width * pixeltob2);
	float32 height = abs(m_scale.y*orb_texture.height * pixeltob2);
	goldenShape.SetAsBox(width/2, height/2);
	goldenFixtureDef.shape = &goldenShape;
	goldenFixtureDef.density = 1;
	goldenFixtureDef.friction = 0.3;

	return true;
}

void GoldenOrb::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	//
	effect.release();
}

void GoldenOrb::update(float ms)
{
	start_time += ms;
	if (start_time >= Gloabal_animation_CountDown) {
		tex_num = (tex_num + 1) % 10;
		start_time = 0.0;
	}

}

void GoldenOrb::draw(const mat3& projection, mat3& view_2d)
{
	if (draw_state == 1){
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
		glBindTexture(GL_TEXTURE_2D, orb_textures[tex_num].id);

		// Setting uniform values to the currently bound program
		glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
		float color[] = { 1.f, 1.f, 1.f };
		glUniform3fv(color_uloc, 1, color);
		glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
		glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);

		// Drawing!
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	}
		
}

vec2 GoldenOrb::get_position()const
{
	return m_position;
}

void GoldenOrb::set_position(vec2 position)
{
	m_position = position;
}

// Returns the local bounding coordinates scaled by the current size of the enemy 
vec2 GoldenOrb::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * 0.4f*orb_texture.width, std::fabs(m_scale.y) * 0.4f*orb_texture.height };
}

bool GoldenOrb::collides_with(Player& enemy)
{
	/*
	float dx = m_position.x - enemy.get_position().x;
	float dy = m_position.y - enemy.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(enemy.get_bounding_box().x - 50.f, enemy.get_bounding_box().y - 50.f);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	r *= 1.f;
	if (d_sq < r * r) {
		return true;
	}
	return false;
	*/
	//return true;

	if (erase_flag == false) { enemy.add_keys(); }
	erase_flag = true;
	return true;
}

sGoldenOrb* GoldenOrb::getInfo() {

	sGoldenOrb* e1 = new sGoldenOrb;
	e1->collided = collided;
	e1->erase_flag = erase_flag;
	e1->start_time = start_time;
	e1->m_position = m_position; // Window coordinates
	e1->m_scale = m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	e1->m_direction = m_direction;
	e1->tex_num = tex_num;
	//e1->goldenBodyDef = goldenBodyDef;
	//e1->goldenShape = goldenShape;
	//e1->goldenFixtureDef = goldenFixtureDef;

	return e1;
}

void GoldenOrb::setInfo(sGoldenOrb* e1) {

	collided = e1->collided;
	erase_flag = e1->erase_flag;
	start_time = e1->start_time;
	m_position = e1->m_position; // Window coordinates
	m_scale = e1->m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	m_direction = e1->m_direction;
	tex_num = e1->tex_num;
	b2Vec2 position = b2Vec2((float32)(m_position.x * pixeltob2), (float32)(m_position.y* pixeltob2));
	rigidbody->SetTransform(position, 0);
	//goldenBodyDef = e1->goldenBodyDef;
	//goldenShape = e1->goldenShape;
	//goldenFixtureDef = e1->goldenFixtureDef;
}

int GoldenOrb::getEntityType()
{
	return ET_goldball;
}

void GoldenOrb::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
	if (r2EntityType == ET_player) {
		collides_with(*((Player*)r2));
		return;
	}
}
