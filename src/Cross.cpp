// Header
#include "Cross.hpp"
// stlib
#include <vector>
#include <string>
#include <algorithm>

bool Cross::init()
{
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;
	bool direction_up = false;
	bool direction_down = false;
	bool direction_left = false;
	bool direction_right = false;


	// Reads the cross mesh from a file, which contains a list of vertices and indices
	FILE* mesh_file = fopen(mesh_path("cross.mesh"), "r");
	if (mesh_file == nullptr)
		return false;

	// Reading vertices and colors
	size_t num_vertices;
	fscanf(mesh_file, "%zu\n", &num_vertices);
	for (size_t i = 0; i < num_vertices; ++i)
	{
		float x, y, z;
		float _u[3]; // unused
		int r, g, b;
		fscanf(mesh_file, "%f %f %f %f %f %f %d %d %d\n", &x, &y, &z, _u, _u + 1, _u + 2, &r, &g, &b);
		Vertex vertex;
		vertex.position = { x, y, z };
		vertex.color = { (float)r / 255, (float)g / 255, (float)b / 255 };
		vertices.push_back(vertex);
	}

	// Reading associated indices
	size_t num_indices;
	fscanf(mesh_file, "%zu\n", &num_indices);
	for (size_t i = 0; i < num_indices; ++i)
	{
		int idx[3];
		fscanf(mesh_file, "%d %d %d\n", idx, idx + 1, idx + 2);
		indices.push_back((uint16_t)idx[0]);
		indices.push_back((uint16_t)idx[1]);
		indices.push_back((uint16_t)idx[2]);
	}

	indicess = {0, 11, 10,  0 , 1, 10,  1, 10 ,7,  1,7,4,  4,7,6,    4,6,5,     10,9,8,    10,7,8,   2,1,4,   2,3,4 };

	// Done reading
	fclose(mesh_file);

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("colored.vs.glsl"), shader_path("colored.fs.glsl")))
		return false;

	// Setting initial values
	m_scale.x = 40.f;
	m_scale.y = 40.f;
	m_num_indices = indices.size();
	//m_position = { -3000.f, 1600.f };
	return true;
}

// Releases all graphics resources
void Cross::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	effect.release();
}

void Cross::draw(const mat3& projection, mat3& view_2d)
{
	transform_begin();
	transform_translate({ m_position.x,m_position.y });
	transform_scale(m_scale);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Getting uniform locations
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
	GLint in_color_loc = glGetAttribLocation(effect.program, "in_color");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_color_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);

	// Cross Color
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);


	// Drawing!
	glDrawElements(GL_TRIANGLES, (GLsizei)m_num_indices, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Cross::get_position()const
{
	return m_position;
}

void Cross::set_position(vec2 position)
{
	m_position = position;
}

void Cross::set_vertex_position() {
	vertices.push_back({ -28,1662 });
	vertices.push_back({ -12,1662 });
	vertices.push_back({ -12,1678 });
	vertices.push_back({ 12,1678 });
	vertices.push_back({ 12,1662 });
	vertices.push_back({ 28,1662 });
	vertices.push_back({ 28,1638 });
	vertices.push_back({ 12,1638 });
	vertices.push_back({ 12,1622 });
	vertices.push_back({ -12,1622 });
	vertices.push_back({ -12,1638 });
	vertices.push_back({ -28,1638 });
}

std::vector<vec2> Cross::get_vertex_position() {
	return vertices;
}


std::vector<vec2> Cross::get_vertex_position2() {
	return vertices2;
}

void Cross::set_vertex_position2() {
	vertices2.push_back({ -428,-1892 });
	vertices2.push_back({ -412,-1892 });
	vertices2.push_back({ -412,-1908 });
	vertices2.push_back({ -388,-1908 });
	vertices2.push_back({ -388,-1892 });
	vertices2.push_back({ -372,-1892 });
	vertices2.push_back({ -372,-1868 });
	vertices2.push_back({ -388,-1868 });
	vertices2.push_back({ -388,-1852 });
	vertices2.push_back({ -412,-1852 });
	vertices2.push_back({ -412,-1868 });
	vertices2.push_back({ -428,-1868 });
}

sCross * Cross::getInfo()
{
	sCross* s1 = new sCross;
	//s1->healthp = healthp;
	s1->m_direction = m_direction;
	s1->m_num_indices = m_num_indices;
	s1->m_position = m_position;
	s1->m_scale = m_scale;
	s1->used = used;
	return s1;
}

void Cross::setInfo(sCross * s1)
{
	m_direction = s1->m_direction;
	m_num_indices = s1->m_num_indices;
	m_position = s1->m_position;
	m_scale = s1->m_scale;
	used = s1->used;
}

void Cross::collidedP(Player & player)
{
	if (player.m_is_alive) {
		if (player.lifePoint < 10 && player.lifePoint > 0 && !used)
		{
			if (player.lifePoint < 10)
				player.lifePoint = player.lifePoint + 5;
			if (player.lifePoint > 10)
				player.lifePoint = 10;
			player.healthp->current_health_level = player.lifePoint;
			used = true;
		}
	}
}

int Cross::getEntityType()
{
	return ET_cross;
}

void Cross::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
	if (r2EntityType == ET_player) {
	
		collidedP(*(Player*)r2);
	}
}

void Cross::setDef()
{
	//set_vertex_position();
	//setting box2d bodydef
	crossBodyDef.type = b2_staticBody;
	float32 x = m_position.x * pixeltob2;
	float32 y = m_position.y  * pixeltob2;
	crossBodyDef.position.Set(x, y);
	crossBodyDef.angle = 0;



	//float32 width = abs(m_scale.x * 14 * pixeltob2);
	//float32 height = abs(m_scale.y * 16 * pixeltob2);
	//b2Vec2 p = b2Vec2(vertices[2].x * pixeltob2 + width, vertices[2].y * pixeltob2 + height);
	/// 0, 1 , 10 ,11

	//float32 width = abs(m_scale.x * 14 * pixeltob2);
	//float32 height = abs(m_scale.y * 16 * pixeltob2);
	//b2Vec2 p1 = b2Vec2(vertices[2].x * pixeltob2 + width, vertices[2].y * pixeltob2 + height);
	/// 4, 5 ,  6 , 7

	//float32 width = abs(m_scale.x * 14 * pixeltob2);
	//float32 height = abs(m_scale.y * 16 * pixeltob2);
	//b2Vec2 p2 = b2Vec2(vertices[2].x * pixeltob2 + width, vertices[2].y * pixeltob2 + height);
	/// 1, 2, 3, 4

	//float32 width = abs(m_scale.x * 14 * pixeltob2);
	//float32 height = abs(m_scale.y * 16 * pixeltob2);
	//b2Vec2 p3 = b2Vec2(vertices[2].x * pixeltob2 + width, vertices[2].y * pixeltob2 + height);
	/// 7, 8 ,9 ,10

	/*
	b2Vec2 vertic[4];
	vertic[3].Set(pixeltob2 * vertices[0].x, pixeltob2 * vertices[0].y );
	vertic[2].Set(pixeltob2 * vertices[1].x, pixeltob2 * vertices[1].y );
	vertic[1].Set(pixeltob2 * vertices[10].x, pixeltob2 * vertices[10].y );
	vertic[0].Set(pixeltob2 * vertices[11].x, pixeltob2 * vertices[11].y );

	b2Vec2 vertic1[4];
	vertic1[3].Set(pixeltob2 * vertices[4].x, pixeltob2 * vertices[4].y );
	vertic1[2].Set(pixeltob2 * vertices[5].x, pixeltob2 * vertices[5].y );
	vertic1[1].Set(pixeltob2 * vertices[6].x, pixeltob2 * vertices[6].y );
	vertic1[0].Set(pixeltob2 * vertices[7].x, pixeltob2 * vertices[7].y );

	b2Vec2 vertic2[4];
	vertic2[3].Set(pixeltob2 * vertices[1].x, pixeltob2 * vertices[1].y );
	vertic2[2].Set(pixeltob2 * vertices[2].x, pixeltob2 * vertices[2].y );
	vertic2[1].Set(pixeltob2 * vertices[3].x, pixeltob2 * vertices[3].y );
	vertic2[0].Set(pixeltob2 * vertices[4].x, pixeltob2 * vertices[4].y );


	b2Vec2 vertic3[4];
	vertic3[3].Set(pixeltob2 * vertices[7].x, pixeltob2 * vertices[7].y );
	vertic3[2].Set(pixeltob2 * vertices[8].x, pixeltob2 * vertices[8].y );
	vertic3[1].Set(pixeltob2 * vertices[9].x, pixeltob2 * vertices[9].y );
	vertic3[0].Set(pixeltob2 * vertices[10].x, pixeltob2 * vertices[10].y);
	*/

	
	/*
	b2Vec2 vertic[12];
	vertic[11].Set(1.5 *  (pixeltob2 * vertices[0].x - x) + x, 1.5 * (pixeltob2 * vertices[0].y - y) + y);
	vertic[10].Set(1.5 *  (pixeltob2 * vertices[1].x - x) + x, 1.5 * (pixeltob2 * vertices[1].y - y) + y);
	vertic[9].Set(1.5 *  (pixeltob2 * vertices[2].x - x) + x, 1.5 * (pixeltob2 * vertices[2].y - y) + y);
	vertic[8].Set(1.5 *  (pixeltob2 * vertices[3].x - x) + x, 1.5 * (pixeltob2 * vertices[3].y - y) + y);
	vertic[7].Set(1.5 *  (pixeltob2 * vertices[4].x - x) + x, 1.5 * (pixeltob2 * vertices[4].y - y) + y);
	vertic[6].Set(1.5 *  (pixeltob2 * vertices[5].x - x) + x, 1.5 * (pixeltob2 * vertices[5].y - y) + y);
	vertic[5].Set(1.5 *  (pixeltob2 * vertices[6].x - x) + x, 1.5 * (pixeltob2 * vertices[6].y - y) + y);
	vertic[4].Set(1.5 *  (pixeltob2 * vertices[7].x - x) + x, 1.5 * (pixeltob2 * vertices[7].y - y) + y);
	vertic[3].Set(1.5 *  (pixeltob2 * vertices[8].x - x) + x, 1.5 * (pixeltob2 * vertices[8].y - y) + y);
	vertic[2].Set(1.5 *  (pixeltob2 * vertices[9].x - x) + x, 1.5 * (pixeltob2 * vertices[9].y - y) + y);
	vertic[1].Set(1.5 * (pixeltob2 * vertices[10].x - x) + x, 1.5 * (pixeltob2 * vertices[10].y - y) + y);
	vertic[0].Set(1.5 * (pixeltob2 * vertices[11].x - x) + x, 1.5 * (pixeltob2 * vertices[11].y - y) + y);
	*/
	
	//setting box2d shape and fixture def

	//float32 width = abs(m_scale.x* cross_texture.width * pixeltob2 * 0.005);
	//float32 height = abs(m_scale.y* player_texture.height * pixeltob2 * 0.005);
	//crossShape.Set(vertic, 12);  

	//crossShape.Set(vertic,4);
	crossShape.SetAsBox(0.25,0.25);
	crossFixtureDef.shape = &crossShape;
	crossFixtureDef.density = 1;
	crossFixtureDef.friction = 0.3;

	/*
	crossShape1.Set(vertic1, 4);
	crossFixtureDef1.shape = &crossShape1;
	crossFixtureDef1.density = 1;
	crossFixtureDef1.friction = 0.3;

	crossShape2.Set(vertic2, 4);
	crossFixtureDef2.shape = &crossShape2;
	crossFixtureDef2.density = 1;
	crossFixtureDef2.friction = 0.3;

	crossShape3.Set(vertic3, 4);
	crossFixtureDef3.shape = &crossShape3;
	crossFixtureDef3.density = 1;
	crossFixtureDef3.friction = 0.3;
	*/


}

