#include "Quest.hpp"
#include "SleepPotion.hpp"

Texture Quest::textures[8];
bool Quest::init(Player* p, bool(*foo)(Player* p), void(*after_complete)(Player* p)) {
	player = p;
	check = foo;
	reward = after_complete;
	Texture tex;
	tex.load_from_file(quest0);
	float wr = tex.width;
	float hr = tex.height;

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

void Quest::destroy() {
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	effect.release();
}

void Quest::update(float ms) {
	complete();
}

void Quest::complete() {
	if (check(player)) {
		completed = true;
		reward(player);
		//do something
	}
}

int Quest::getEntityType() {
	return 0;
}

void Quest::handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType) {
}

//sQuest * Quest::getInfo()
//{
//	sQuest* q1 = new sQuest;
//	q1->completed = completed;
//	q1->texnum = texnum;
//	return q1;
//}
//
//void Quest::setInfo(sQuest * q1)
//{
//	completed = q1->completed;
//	texnum = q1->texnum;
//}

void Quest::draw(const mat3& projection, mat3& view_2d) {
	transform_begin();
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
	glBindTexture(GL_TEXTURE_2D, textures[texnum].id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

bool check_1(Player* player) {
	return (player->get_keys()==1);
}

bool check_2(Player* player) {
	return (player->get_exp() > 200);
}

bool check_3(Player* player) {
	return (player->get_keys()==4);
}

bool check_4(Player* player) {
	return (player->get_keys() == 4);
}

bool check_5(Player* player) {
	return (player->get_keys() == 4);
}

bool check_6(Player* player) {
	if (player->check_near_npc()) return true;  //change it if npc position fixed
	else return false;
}
bool check_7(Player* player) {
	return player->get_exp() > 5000;
}
void reward_1(Player* player) {
	player->add_exp(5);
}

void reward_6(Player* player) {
	player->can_use_item = true;
	player->add_exp(1000);
}


Texture QuestManager::texture;
bool QuestManager::init(Player* player) {
	Quest q0;
	Quest q1;
	Quest q2;
	Quest q3;
	q0.init(player, &check_1, &reward_1);
	q1.init(player, &check_2, &reward_1);
	q2.init(player, &check_3, &reward_1);
	q3.init(player, &check_4, &reward_1);
	emptyQuest.init(player, &check_1, &reward_1);
	emptyQuest.texnum = 7;
	q0.texnum = 0;
	q1.texnum = 1;
	q2.texnum = 2;
	q3.texnum = 3;
	quests.emplace_back(q0);
	quests.emplace_back(q1);
	quests.emplace_back(q2);
	quests.emplace_back(q3);
	p = player;
	current = 0;
	Texture tex;
	tex.load_from_file(questmanager0);
	float wr = tex.width;
	float hr = tex.height;

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
	nq.init();
	return true;
}

void QuestManager::destroy() {
	for (auto& q : quests) {
		q.destroy();
	}
	nq.destroy();
	emptyQuest.destroy();
	quests.clear();
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	effect.release();
}

void QuestManager::update(float ms) {
	if (enlarge) nq.show = false;
	if (nq.show) nq.update(ms);
	if (current >= quests.size()||current >= 7) return;
	quests[current].update(ms);
	if (quests[current].completed) {
		current++;
		nq.show = true;
	}
}

bool QuestManager::get_empty() {
	return current >= quests.size();
}

void QuestManager::draw(const mat3& projection, mat3& view_2d) {
	mat3 md = mat3({ { -0.001f, 0.f, 0.f },{ 0.f, -0.001f, 0.f },{ 0.f, 0.9f, 1.f } });
	transform_begin();
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
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&md);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	mat3 m = mat3({ { -0.0001f, 0.f, 0.f },{ 0.f, -0.0001f, 0.f },{ 0.f, 0.8f, 1.f } });
	if (nq.show) nq.draw(mat3({ { 0.001f, 0.f, 0.f },{ 0.f, -0.001f, 0.f },{ 0.f, 0.7f, 1.f } }), view_2d);
	if (enlarge) m = mat3({ { -0.0015f, 0.f, 0.f },{ 0.f, -0.002f, 0.f },{ 0.f, 0.f, 1.f } });
	if (current >= quests.size() || current >= 7) emptyQuest.draw(m, view_2d);
	else quests[current].draw(m,view_2d);
}

int QuestManager::getEntityType() {
	return 0;
}

void QuestManager::handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType) {
}

void QuestManager::insert_quest() {
	Quest q4;
	Quest q5;
	Quest q6;
	q4.init(p, &check_5, &reward_1);
	q5.init(p, &check_6, &reward_6);
	q6.init(p, &check_7, &reward_1);
	q4.texnum = 4;
	q5.texnum = 5;
	q6.texnum = 6;
	quests.emplace_back(q4);
	quests.emplace_back(q5);
	quests.emplace_back(q6);
	nq.show = true;
	inserted1 = true;
}

sQuestManager * QuestManager::getInfo()
{
	sQuestManager* qm = new sQuestManager;
	qm->current = current;
	qm->inserted1 = inserted1;
	return qm;
}

void QuestManager::setInfo(sQuestManager * qm)
{
	current = qm->current;
	inserted1 = qm->inserted1;
}

Texture NewQuest::texture[6];
bool NewQuest::init()
{
	// Load shared texture
	Texture skill_texture;
	skill_texture.load_from_file(newquest0);

	// The position corresponds to the center of the texture
	float wr = skill_texture.width;
	float hr = skill_texture.height;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.01f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.01f };
	vertices[1].texcoord = { 1.f, 1.f, };
	vertices[2].position = { +wr, -hr, -0.01f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.01f };
	vertices[3].texcoord = { 0.f, 0.f };

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

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

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture

	return true;
}

// Call if init() was successful
// Releases all graphics resources
void NewQuest::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	effect.release();
}

void NewQuest::update(float ms)
{
	// Move npc along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	start_time += ms;
	if (start_time >= Gloabal_animation_CountDown) {
		tex_num = (tex_num + 1) % 6;
		start_time = 0.0;
	}
	//m_position.x += step;
}

void NewQuest::draw(const mat3& projection, mat3& view_2d)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
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
	glBindTexture(GL_TEXTURE_2D, texture[tex_num].id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(view_uloc, 1, GL_FALSE, (float*)&view_2d);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

//sNewQuest * NewQuest::getInfo()
//{
//	sNewQuest* n1 = new sNewQuest;
//	n1->show = show;
//	n1->start_time = start_time;
//	n1->tex_num = tex_num;
//	return n1;
//}
//
//void NewQuest::setInfo(sNewQuest * n1)
//{
//	show = n1->show;
//	start_time = n1->start_time;
//	tex_num = n1->tex_num;
//}

int NewQuest::getEntityType()
{
	return ET_fire;
}

void NewQuest::handleCollision(Renderable * r1, Renderable * r2, int r1EntityType, int r2EntityType)
{
}
