#include "SleepPotion.hpp"

Texture SleepPotion::sleep_tex;
bool SleepPotion::init(vec2 position, vec2 direction, float sleeptime, float cooldown) {

	char texPath[] = character_attack;
	char texName[] = "0.png";
	char buffer[256];
	strncpy(buffer, texPath, sizeof(buffer));
	strncat(buffer, texName, sizeof(buffer));
	texture.load_from_file(buffer);

	Life_time = 800.f;
	consumed_time = 0.f;

	float wr = texture.width*0.02f;
	float hr = texture.height*0.02f;

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


	//AABB* ab = coll.init({vertices[2].position.x, vertices[2].position.y}, { vertices[0].position.x, vertices[0].position.y }, attack_p);



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
	m_scale.x = 15.f;
	if (direction.x > 0) m_scale.x = -m_scale.x;
	m_scale.y = 15.f;
	inited = true;
	sleep_time = sleeptime;
	cd = cooldown;

	texture = sleep_tex;

	return true;
}

void SleepPotion::function(Dragon* dragon) {
	dragon->set_sleep(sleep_time);
}

void SleepPotion::update(float ms) {
	if (consumed_time >= Life_time) {
		m_scale.x = 0.f;
		m_scale.y = 0.f;
		m_position = vec2{ 0.f, 0.f };
		inited = false;
	}
	else {
		consumed_time += ms;
		m_position.x += m_direction.x*ms;
		m_position.y += m_direction.y*ms;
		rotation += acos(-1) / 30;
	}
	if (cd > 0.f) cd -= ms;

	b2Vec2 b2position = b2Vec2((float32)(m_position.x * pixeltob2), (float32)(m_position.y* pixeltob2));
	if (rigidbody) { rigidbody->SetTransform(b2position, 0); }
}

int SleepPotion::getEntityType() {
	return ET_sleep_position;
}
void SleepPotion::addDef()
{
	//setting box2d bodydef
	sleepBodyDef.type = b2_dynamicBody;
	float32 x = m_position.x * pixeltob2;
	float32 y = m_position.y * pixeltob2;
	sleepBodyDef.position.Set(x, y);
	sleepBodyDef.angle = 0;
	//setting box2d shape and fixture def
	//float32 width = abs(m_scale.x* texture.width * pixeltob2 * 0.04);
	//float32 height = abs(m_scale.y* texture.height * pixeltob2 * 0.04);
	sleepShape.SetAsBox(1.0f, 1.0f);
	sleepFixtureDef.shape = &sleepShape;
	sleepFixtureDef.density = 1;
	sleepFixtureDef.friction = 0.3;

}
void SleepPotion::handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType) {

	if (r2EntityType == ET_dragon) {

		function((Dragon*)(r2));
		printf("sleeped \n");
	}
};