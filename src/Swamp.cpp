#include <vector>
#include <string>
#include <algorithm>
#include "Swamp.hpp"

Texture Swamp::swamp_textures[10];

bool Swamp::init(vec2 position) {
	char texPath[] = ground_swamp;
	char texName[] = "0.png";
	char buffer[256];
	strncpy(buffer, texPath, sizeof(buffer));
	strncat(buffer, texName, sizeof(buffer));
	swamp_texture.load_from_file(buffer);

	float wr = swamp_texture.width;
	float hr = swamp_texture.height;

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

	m_scale.x = 0.7;
	m_scale.y = 0.03;
	//tex_num = 0;

	vertex = { vec2{vertices[0].position.x*m_scale.x,vertices[0].position.y*m_scale.y}
,vec2{vertices[1].position.x*m_scale.x,vertices[1].position.y*m_scale.y}
,vec2{vertices[2].position.x*m_scale.x,vertices[2].position.y*m_scale.y}
,vec2{vertices[3].position.x*m_scale.x,vertices[3].position.y*m_scale.y} };

	return true;
}

void Swamp::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	effect.release();
}

void Swamp::draw(const mat3& projection, mat3& view_2d)
{
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
	glBindTexture(GL_TEXTURE_2D, swamp_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);


	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Swamp::update(float ms)
{
	//b2Vec2 b2position = b2Vec2((float32)(m_position.x * pixeltob2), (float32)(m_position.y* pixeltob2));
	//if (rigidbody) { rigidbody->SetTransform(b2position, 0); }
}

void Swamp::collide_with(Player & player)
{
}

vec2 Swamp::get_position()
{
	return m_position;
}

int Swamp::getEntityType()
{
	return ET_swamp;
}

void Swamp::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
	if (r2EntityType == ET_player) {
	
	
 		((Player*)r2)->swampContacting = true;
		//printf("player stand on \n");

		b2Vec2 position = ((Player*)r2)->rigidbody->GetPosition();
		//float32 angle = ballBody->GetAngle();
		//printf("%4.2f %4.2f \n", position.x, position.y);
	}

}

void Swamp::setDef()
{
	//swampBodyDef.type = b2_staticBody;
	float32 x = m_position.x * pixeltob2;
	float32 y = m_position.y * pixeltob2;
	swampBodyDef.position.Set(0, 0);
	swampBodyDef.gravityScale = 0.0f;
	//swampBodyDef.position.Set(x, y);
	//swampBodyDef.angle = 0;
	//setting box2d shape and fixture def
	float32 width = 2.0 * abs(m_scale.x* swamp_texture.width * pixeltob2) / 2;
	//float32 height = abs(m_scale.y* venom_texture.height * pixeltob2 * 2);
	//venomShape.SetAsBox(width / 2, height / 2);
	b2Vec2 startpoint = b2Vec2(float32(x- width), float32(y - 0.8f));
	b2Vec2 endpoint = b2Vec2(float32(x+ width),float32(y - 0.8f));

	//b2Vec2 startpoint = b2Vec2(-75.f, -1.3f);
	//b2Vec2 endpoint = b2Vec2(-25.f, -1.3f);

	b2Vec2 vertex[2];
	vertex[1].Set(startpoint.x,startpoint.y);
	vertex[0].Set(endpoint.x, endpoint.y);


	swampShape.CreateChain(vertex,2);
	swampShape.SetNextVertex(b2Vec2(endpoint.x + 2, endpoint.y));
	swampShape.SetPrevVertex(b2Vec2(startpoint.x - 1, startpoint.y));




	swampFixtureDef.shape = &swampShape;
	swampFixtureDef.density = 1;
	swampFixtureDef.friction = 0.3;
	//printf("%4.2f %4.2f \n", x, endpoint.y);
}
