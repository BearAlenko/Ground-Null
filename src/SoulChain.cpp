#include "SoulChain.hpp"

Texture SoulChain::texture;
bool SoulChain::init(Player* player, Enemy* enemy) {
	p = player;
	e = enemy;
	count_down = 800.f;
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
	//indicess = { indices[0],indices[1],indices[2],indices[3],indices[4],indices[5] };

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
	return true;
}

void SoulChain::start() {
	float dir_x = p->get_position().x - e->get_position().x;
	float dir_y = p->get_position().y - e->get_position().y;
	vec2 dir = vec2{ dir_x, dir_y };
	dir = normalize(dir);
	float radians = atan2f(dir.y, dir.x);
	particle_effect.init(e->get_position(), radians, normal_particle, 15, &p->m_position);
}

void SoulChain::check_enough_lv() {
	if (p->get_level() >= 7) enough_lv = true;
	else enough_lv = false;
}

void SoulChain::update(float ms) {
	vec2 p_p = p->get_position();
	vec2 p_e = e->get_position();
	m_position = p_p;
	float distance_x = p_e.x - p_p.x;
	float distance_y = p_e.y - p_p.y;
	vec2 d = vec2{ distance_x, distance_y };
	float distance = dot(d, d);
	if (distance > 1000000.f) {
		in_range = false;
		activated = false;
	}
	if (in_range && activated) {
		if (count_down <= 0.f) {
			p->lifePoint += 1;
			if(p->lifePoint > p->max_hp) p->lifePoint = p->max_hp;
			e->hp -= 1;
			count_down = 800.f;
			p->healthp->current_health_level = p->lifePoint;
			e->healthpoint->current_health_level = e->hp;
		}
		count_down -= ms;
		float dir_x = p->get_position().x - e->get_position().x;
		float dir_y = p->get_position().y - e->get_position().y;
		vec2 dir = vec2{ dir_x, dir_y };
		dir = normalize(dir);
		float radians = atan2f(dir.y, dir.x);
		particle_effect.init_position = e->get_position();
		particle_effect.init_dir = radians;
		particle_effect.update(ms);
		float pi = acos(-1);
		m_rotation += pi / 60;
		if (m_rotation >= 2 * pi) m_rotation = 0.f;
		e->chained = true;
	}
	else e->chained = false;
}

int SoulChain::getEntityType()
{
	return ET_particle;
}

void SoulChain::set_enemy(Enemy* en) {
	e = en;
}

void SoulChain::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
}

void SoulChain::destroy() {
	glDeleteBuffers(1, &mesh.vbo);

	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	in_range = false;
	activated = false;
	effect.release();
	particle_effect.destroy();
}

void SoulChain::draw(const mat3& projection, mat3& view_2d) {
	if (in_range && activated) {
		particle_effect.draw(projection, view_2d);
		transform_begin();
		transform_translate(m_position);
		transform_rotate(m_rotation);
		transform_scale({ m_scale, m_scale });
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
		glBindTexture(GL_TEXTURE_2D, texture.id);

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