#include "ParticleEffect.hpp"
#include <iostream>
using namespace std;

bool ParticleEffect::init(vec2 position, float direction, const char* fileName, int max, vec2* dest) {
	init_position = position;
	init_dir = direction;
	max_particles = max;
	emitter.Origin = position;
	tex.load_from_file(fileName);
	destination = dest;
	destroy();
	EmitParticles();
	/*TexturedVertex vertices[80];
	uint16_t indices[120];
	float time_buffer[120];
	vec2 position_buffer[120];
	for(int i = 0; i < m_Particles.size(); i++) {
		vertices[i * 4 + 0] = m_Particles[i].vertices[0];
		vertices[i * 4 + 1] = m_Particles[i].vertices[1];
		vertices[i * 4 + 2] = m_Particles[i].vertices[2];
		vertices[i * 4 + 3] = m_Particles[i].vertices[3];
		indices[i * 6 + 0] = m_Particles[i].indices[0]+i*6;
		indices[i * 6 + 1] = m_Particles[i].indices[1] + i * 6;
		indices[i * 6 + 2] = m_Particles[i].indices[2] + i * 6;
		indices[i * 6 + 3] = m_Particles[i].indices[3] + i * 6;
		indices[i * 6 + 4] = m_Particles[i].indices[4] + i * 6;
		indices[i * 6 + 5] = m_Particles[i].indices[5] + i * 6;
		time_buffer[i * 6 + 0] = m_Particles[i].time_buffer[0];
		time_buffer[i * 6 + 1] = m_Particles[i].time_buffer[1];
		time_buffer[i * 6 + 2] = m_Particles[i].time_buffer[2];
		time_buffer[i * 6 + 3] = m_Particles[i].time_buffer[3];
		time_buffer[i * 6 + 4] = m_Particles[i].time_buffer[4];
		time_buffer[i * 6 + 5] = m_Particles[i].time_buffer[5];
		position_buffer[i * 6 + 0] = m_Particles[i].m_Position;
		position_buffer[i * 6 + 1] = m_Particles[i].m_Position;
		position_buffer[i * 6 + 2] = m_Particles[i].m_Position;
		position_buffer[i * 6 + 3] = m_Particles[i].m_Position;
		position_buffer[i * 6 + 4] = m_Particles[i].m_Position;
		position_buffer[i * 6 + 5] = m_Particles[i].m_Position;
	}// Clearing error
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 80, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 120, indices, GL_STATIC_DRAW);

	glGenBuffers(1, &time_buffer_loc);
	glBindBuffer(GL_ARRAY_BUFFER, time_buffer_loc);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 120, time_buffer, GL_STATIC_DRAW); //change if want dynamic

	glGenBuffers(1, &position_buffer_loc);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer_loc);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 120, position_buffer, GL_STATIC_DRAW); //change if want dynamic

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;
	if (!effect.load_from_file(shader_path("particle2.vs.glsl"), shader_path("particle2.fs.glsl")))
		return false;*/
	m_Force = { 0.f, 0.f };
	return true;
}

void ParticleEffect::destroy()
{
	for (auto& particle : m_Particles) {
		particle.destroy();
	}
	m_Particles.clear();
}

void ParticleEffect::update(float ms) {
	/*auto it = m_Particles.begin();
	while (it != m_Particles.end()) {
		(*it)->update(ms);
		if ((*it)->m_consumedTime >= (*it)->m_LifeTime) {
			(*it)->destroy();
			delete (*it);
			it = m_Particles.erase(it);
		}
		else ++it;
	}*/
	for (auto& particle : m_Particles) {
		particle.update(ms);
		if (particle.m_consumedTime >= particle.m_LifeTime) {
			particle.reset(init_position, init_dir, tex);
			emitter.EmitParticle(particle);
		}
	}
}

void ParticleEffect::draw(const mat3& projection, mat3& view_2d) {
	for (auto& particle : m_Particles) {
		particle.draw(projection, view_2d);
	}
	/*
	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	//GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint view_uloc = glGetUniformLocation(effect.program, "view");
	//GLint time_uloc = glGetUniformLocation(effect.program, "time");
	//GLint life_time_uloc = glGetUniformLocation(effect.program, "life_time");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, time_buffer_loc);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer_loc);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	GLint in_time_loc = glGetAttribLocation(effect.program, "in_time");
	GLint in_transform_loc = glGetAttribLocation(effect.program, "in_transform");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glEnableVertexAttribArray(in_time_loc);
	glEnableVertexAttribArray(in_transform_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex)+sizeof(float)+sizeof(vec2), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex) + sizeof(float) + sizeof(vec2), (void*)sizeof(vec3));
	glVertexAttribPointer(in_time_loc, 1, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex) + sizeof(float) + sizeof(vec2), (void*)sizeof(TexturedVertex));
	glVertexAttribPointer(in_transform_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex) + sizeof(float) + sizeof(vec2), (void*)(sizeof(TexturedVertex)+sizeof(float)));


	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Particle::normal_particle_tex.id);

	// Setting uniform values to the currently bound program
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);*/
}

void ParticleEffect::EmitParticles() {
	while (m_Particles.size() < max_particles) {
		Particle newparticle;
		newparticle.init(init_position, init_dir, tex);
		newparticle.destination = destination;
		emitter.EmitParticle(newparticle);
		m_Particles.push_back(newparticle);
	}
}
