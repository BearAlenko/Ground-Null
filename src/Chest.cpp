#include <vector>
#include <string>
#include <algorithm>
#include "Chest.hpp"

Texture Chest::chest_textures[10];


bool Chest::init(vec2 position) {
	char texPath[] = ground_chest;
	char texName[] = "0.png";
	char buffer[256];
	strncpy(buffer, texPath, sizeof(buffer));
	strncat(buffer, texName, sizeof(buffer));
	chest_texture.load_from_file(buffer);

	float wr = chest_texture.width;
	float hr = chest_texture.height;

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

	m_scale.x = 0.2;
	m_scale.y = 0.2;
	tex_num = 0;

	vertex = { vec2{vertices[0].position.x*m_scale.x,vertices[0].position.y*m_scale.y}
,vec2{vertices[1].position.x*m_scale.x,vertices[1].position.y*m_scale.y}
,vec2{vertices[2].position.x*m_scale.x,vertices[2].position.y*m_scale.y}
,vec2{vertices[3].position.x*m_scale.x,vertices[3].position.y*m_scale.y} };

	return true;
}

void Chest::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	effect.release();
}

void Chest::draw(const mat3& projection, mat3& view_2d)
{
	if(lockLife > 0){
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
		glBindTexture(GL_TEXTURE_2D, chest_textures[tex_num].id);

		// Setting uniform values to the currently bound program
		glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
		float color[] = { 1.f, 1.f, 1.f };
		glUniform3fv(color_uloc, 1, color);
		glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
		glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);


		// Drawing!
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	}
	else {
		erase_flag = true;
	}
	
}

vec2 Chest::get_position()
{
	return m_position;
}

int Chest::getEntityType()
{
	return ET_chest;
}

void Chest::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
	if (r2EntityType == ET_player_attack) {
	
		//collide_with(*(Attack_P*)r2);
		r2->handleCollision(r2, r1, r2EntityType, r1EntityType);
	
	}
	else if (r2EntityType == ET_player) {
	
		collidedPlayer = true;
	}
}

void Chest::setDef()
{
	
	//setting box2d bodydef
	chestBodyDef.type = b2_staticBody;
	float32 x = m_position.x * pixeltob2;
	float32 y = m_position.y * pixeltob2;
	chestBodyDef.position.Set(x, y);
	chestBodyDef.angle = 0;
	//setting box2d shape and fixture def
	float32 width = abs(m_scale.x* chest_texture.width * pixeltob2 * 1.2);
	float32 height = abs(m_scale.y* chest_texture.height * pixeltob2 * 0.5);
	chestShape.SetAsBox(width / 2, height / 2);
    chestFixtureDef.shape = &chestShape;
	chestFixtureDef.density = 1;
	chestFixtureDef.friction = 0.3;
	
}

GoldenOrb* Chest::broken() {
	GoldenOrb* go = new GoldenOrb;
	go->init(m_position);
	return go;
}

void Chest::update(float ms) {
	if (lockLife == 1) { 
		tex_num = 1; 
	}
	if (collidedPlayer) {
	
		CheckPlayer();
	}
}

void Chest::collide_with_Player(Player & player)
{
	//////do things with player like bouncing
	///*if (player.get_position().x <= m_position.x - chest_texture.width / 2 * m_scale.x && player.get_position().y + player.get_texture_height() / 2 * player.get_m_scale().y >= m_position.y - chest_texture.height / 2 * m_scale.y  ) {
	//	player.move({ -6.8f,0.f });
	//}*/
	//if (player.get_position().x <= m_position.x - chest_texture.width / 2 * m_scale.x) {
	//	player.move({ -6.8f,0.f });
	//}
	//if (player.get_position().x >= m_position.x + chest_texture.width / 2 * m_scale.x) {
	//	player.move({ 6.8f,0.f });
	//}
}

sChest * Chest::getInfo()
{
	sChest* c = new sChest;
	c->lockLife = lockLife;
	c->m_position = m_position;
	return c;
}

void Chest::setInfo(sChest * c)
{
	lockLife = c->lockLife;
	m_position = c->m_position;
}

void Chest::CheckPlayer()
{
	for (b2ContactEdge* ce = rigidbody->GetContactList(); ce; ce = ce->next)
	{

		void* bodyUserData = ce->other->GetUserData();
		if (bodyUserData) {

			int Bentitytype = static_cast<Renderable*>(bodyUserData)->getEntityType();

			if (Bentitytype == ET_player && ce->contact->IsTouching())
			{

				Player* player = static_cast<Player*>(bodyUserData);


				collide_with_Player(*player);

				return;
			}
		}


	}

	collidedPlayer = false;

}

/*
bool Chest::collide_with(Attack_P & attack_p)
{
	if(attack_p.hit =)
	return false;
}*/
