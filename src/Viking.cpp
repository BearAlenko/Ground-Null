// Header
#include "Viking.hpp"
#include <vector>
#include <cmath>
#include <time.h>

Texture Viking::run_Textures[10];
Texture Viking::attack_Textures[10];

bool Viking::newInit(Player* player, std::vector<Fire>* m_fire, HealthPoints* heathp) {
	init(viking_run, player, m_fire, heathp);
	exp = 8;
	hp = 3;
	max_step += 0.5f;
	type = 1;

	//setting box2d bodydef
	vikingBodyDef.type = b2_dynamicBody;
	float32 x = m_position.x * pixeltob2;
	float32 y = m_position.y * pixeltob2;
	vikingBodyDef.position.Set(x, y);
	vikingBodyDef.angle = 0;
	//setting box2d shape and fixture def
	float32 width = abs(m_scale.x* enemy_texture.width * pixeltob2 * 0.25);
	float32 height = abs(m_scale.y* enemy_texture.height * pixeltob2 * 0.4);
	vikingShape.SetAsBox(width / 2, height / 2);
	vikingFixtureDef.shape = &vikingShape;
	vikingFixtureDef.density = 1;
	vikingFixtureDef.friction = 0.3;

	return true;

}

int Viking::getEntityType()
{
	return ET_viking;
}

void Viking::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
	if (r2EntityType == ET_player) {
		((Player*)r2)->enemyContacting++;
	}
	else if (r2EntityType == ET_player_attack || r2EntityType == ET_player_shield || r2EntityType == ET_player_flare){


		r2->handleCollision(r2, r1, r2EntityType, r1EntityType);
	}
	else if (r2EntityType == ET_ground) {




		ground_height = ((Ground*)r2)->Gstartpoint.y * b2topixel;
		groundContacting++;
	}
}

void Viking::draw(const mat3& projection, mat3& view_2d)
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
	glActiveTexture(GL_TEXTURE0);
	if (attack)
	{
		glBindTexture(GL_TEXTURE_2D, attack_Textures[tex_num].id);
		count--;
		if (count == 0)
		{
			attack = false;
			count = 30;
		}

	}
	else if (run)
		glBindTexture(GL_TEXTURE_2D, run_Textures[tex_num].id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniform1f(distort_uloc, distort);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}
