#include <vector>
#include <string>
#include <algorithm>
#include "Pillar.hpp"

Texture Pillar::pillar_textures[3];

bool Pillar::init(vec2 position) {
	char texPath[] = ground_pillar;
	char texName[] = "0.png";
	char buffer[256];
	strncpy(buffer, texPath, sizeof(buffer));
	strncat(buffer, texName, sizeof(buffer));
	pillar_texture.load_from_file(buffer);

	float wr = pillar_texture.width;
	float hr = pillar_texture.height;

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
	m_position = position;

	m_scale.x = 0.3;
	m_scale.y = 0.2;
	tex_num = 0;

	vertex = { vec2{vertices[0].position.x*m_scale.x,vertices[0].position.y*m_scale.y}
,vec2{vertices[1].position.x*m_scale.x,vertices[1].position.y*m_scale.y}
,vec2{vertices[2].position.x*m_scale.x,vertices[2].position.y*m_scale.y}
,vec2{vertices[3].position.x*m_scale.x,vertices[3].position.y*m_scale.y} };

	return true;
}

void Pillar::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	effect.release();
}

void Pillar::draw(const mat3& projection, mat3& view_2d)
{
	if (lockLife > 0) {
		transform_begin();
		transform_translate({ m_position.x,m_position.y });
		transform_scale(m_scale);
		transform_end();

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
		glBindTexture(GL_TEXTURE_2D, pillar_textures[tex_num].id);

		// Setting uniform values to the currently bound program
		glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
		float color[] = { 1.f, 1.f, 1.f };
		glUniform3fv(color_uloc, 1, color);
		glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
		glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);


		// Drawing!
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	}
	else
		erase_flag = true;
	
}

vec2 Pillar::get_position()
{
	return m_position;
}

int Pillar::getEntityType()
{
	return ET_pillar;
}

void Pillar::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
	if (r2EntityType == ET_player_attack) {
	
		r2->handleCollision(r2, r1, ET_player_attack, ET_pillar);

	
	}
	else if (r2EntityType == ET_player) {
	
	
		collide_player = true;
	}
}


void Pillar::setDef()
{
	//setting box2d bodydef
	pillarBodyDef.type = b2_staticBody;
	float32 x = m_position.x * pixeltob2;
	float32 y = m_position.y * pixeltob2;
    pillarBodyDef.position.Set(x, y);
	pillarBodyDef.angle = 0;
	//setting box2d shape and fixture def
	float32 width = abs(m_scale.x* pillar_texture.width * pixeltob2 * 0.6);
	float32 height = abs(m_scale.y* pillar_texture.height * pixeltob2);
	pillarShape.SetAsBox(width / 2, height / 2);
	pillarFixtureDef.shape = &pillarShape;
	pillarFixtureDef.density = 1;
    pillarFixtureDef.friction = 0.3;
}

void Pillar::collide_with(Player & player)
{
	///// do stuff with player like bouncing
	if (player.get_position().x < m_position.x && lockLife > 0) {
		player.move({ -6.8f,0.f });
	}
	if(player.get_position().x > m_position.x && lockLife > 0){
		player.move({6.8f,0.f});
	}
	
}

void Pillar::check_Player()
{
	for (b2ContactEdge* ce = rigidbody->GetContactList(); ce; ce = ce->next)
	{

		void* bodyUserData = ce->other->GetUserData();
		if (bodyUserData) {

			int Bentitytype = static_cast<Renderable*>(bodyUserData)->getEntityType();

			if (Bentitytype == ET_player && ce->contact->IsTouching())
			{

				Player* player = static_cast<Player*>(bodyUserData);


				collide_with(*player);

				return;
			}
		}


	}

	collide_player = false;

}

sPillar * Pillar::getInfo()
{
	sPillar* sp = new sPillar;
	sp->lockLife = lockLife;
	sp->m_position = m_position;
	return sp;
}

void Pillar::setInfo(sPillar * sp)
{
	lockLife = sp->lockLife;
	m_position = sp->m_position;
}

void Pillar::update(float ms){
	if (lockLife == 2) { tex_num = 1; }
	if (lockLife == 1) { tex_num = 2; }

	if (collide_player) {

		check_Player();
	}
}