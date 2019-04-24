// Header
#include "enemy.hpp"
#include <cmath>
#include <time.h>


// Call if init() was successful
// Releases all graphics resources
Mix_Chunk* Enemy::m_enemy_attack_sound;

bool Enemy::init(char enemy[], Player* player, std::vector<Fire>* m_fire, HealthPoints* healthp)
{
	char texPath[256] = "";
	strncpy(texPath, enemy, sizeof(texPath));
	char texName[] = "0.png";
	char buffer[256];
	strncpy(buffer, texPath, sizeof(buffer));
	strncat(buffer, texName, sizeof(buffer));
	enemy_texture.load_from_file(buffer);

	this->player = player;
	fires = m_fire;
	healthpoint = healthp;
	physic.init(true);
	physic.face_right = false;
	sleep_time = 0.f;
	//exp = 0;
	up = false;
	down = false;
	left = false;
	run = true;
	attack = false;
	dead = false;
	animation_done = true;
	touch_fire = false;
	range = false;
	boss = false;
	canAttack = false;
	healthy = false;
	rage = false;
	cd = false;
	fly = false;
	dragon = false;
	start_time = 0.0;
	tex_num = 0;
	run_tex = 0;
	action_count = 0;
	step = 6.0;
	//facing = 1;
	speed_add = 0.0;
	jump_speed = 8.0;
	max_step = player_run_stepMax;

	// The position corresponds to the center of the texture
	float wr = enemy_texture.width * 0.5f;
	float hr = enemy_texture.height * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.02f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.02f };
	vertices[1].texcoord = { 1.f, 1.f };
	vertices[2].position = { +wr, -hr, -0.02f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.02f };
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
	if (!effect.load_from_file(shader_path("enemy.vs.glsl"), shader_path("enemy.fs.glsl")))
		return false;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_scale.x = 0.45f;
	m_scale.y = 0.45f;
	m_rotation = 0.f;
	hp = 3;



	vertex = { vec2{vertices[0].position.x*m_scale.x,vertices[0].position.y*m_scale.y}
	,vec2{vertices[1].position.x*m_scale.x,vertices[1].position.y*m_scale.y}
	,vec2{vertices[2].position.x*m_scale.x,vertices[2].position.y*m_scale.y}
	,vec2{vertices[3].position.x*m_scale.x,vertices[3].position.y*m_scale.y} };

	indicess = { indices[0],indices[1],indices[2],indices[3],indices[4],indices[5] };


	

	return true;
}

void Enemy::destroy()
{

	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	effect.release();
}

void Enemy::update(float ms)
{
	// Move npc along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	start_time += ms;
	cd = count;
	check_range();
	check_healthy();
	check_canAttack();
	check_touch_fire();
	check_animation_done();
	check_dead();
	check_left();
	if (m_position.y >= 1800.f) hp = 0;
	if (enemy_sound_countdown != 0)
		enemy_sound_countdown--;
	if (dragon_sound_countdown != 0)
		dragon_sound_countdown--;
	vec2 motion = physic.cal_Force(General_friction);
	if (abs(motion.x) > 0.6f* max_step) motion.x = motion.x / abs(motion.x) * 0.6f*max_step;  // horizontal motion can't exceed max s
	if (abs(motion.y) > player_jump_stepMax) motion.y = motion.y / abs(motion.y) * player_jump_stepMax;  // horizontal motion can't exceed max s


	/*float bot_position = ground_height;
	if (dragon) bot_position = ground_height-300.f;*/


	b2Vec2 b2position = b2Vec2((float32)(m_position.x * pixeltob2), (float32)(m_position.y* pixeltob2));
	if (rigidbody) { rigidbody->SetTransform(b2position, 0); }

	if (groundContacting) {
		//check_ground();
	}

	float bot_position = ground_height;
	if (dragon) bot_position = ground_height - 2000.f;
	if (dragon && sleep_time > 0.f) sleep_time -= ms;

	if (m_position.y >= bot_position) {          // check in the air
		physic.support_force = vec2({ 0.f, -(abs(motion.y) + Global_gravity) });
		m_position.y = bot_position;
		physic.ground = true;
	}
	else {
		physic.support_force = vec2({ 0.f, 0.f });
		physic.ground = false;
	}
	move(vec2{ motion.x * ms / 10.f, motion.y * ms / 20.f });



	for (auto& force : forces_to_delete) {
		physic.delete_Force(force);
	}
	forces_to_delete.clear();

	if (boss == true) {
	}
	else {
	}
	if (start_time >= Gloabal_animation_CountDown) {
		if (boss == true) {

			if (rage == true && !dragon)
			{
				rageColor = { 1.f,0.f,0.f };
			}
		}
		if (!dragon) {
			if (attack && count == 30)
			{
				tex_num = 0;
			}
			else if (boss == true && attack == false)
				run_tex = (run_tex + 1) % 16;
			else
				tex_num = (tex_num + 1) % 10;
		}
		else {
			if (attack && action_count == 33)
			{
				tex_num = 0;
			}
			else if (!attack || fly) {
				run_tex = (run_tex + 1) % 10;
				if (run_tex == 0) run_tex = 1;
			}
			else tex_num = (tex_num + 1) % 33;
		}
		int ac = 20;
		if (dragon) ac = 66;
		if (attack && action_count < ac) action_count++;


		start_time = 0.0;
		// if (attack) {                                                // if it runs set the path to run
		//	enemy_texture = attack_Textures[tex_num];
		//}
		// else if (run)
		//	enemy_texture = run_Textures[tex_num];
	}
	healthpoint->set_position({ get_position().x, get_position().y - 150 });
	dragon? m_scale.x = 1.f:m_scale.x = 0.45f;
	dragon ? m_scale.y = 1.f : m_scale.y = 0.45f;
	if (chained) {
		distort += ms;
		m_scale.x = 0.90f*m_scale.x + 0.1f*abs(cosf(distort / 300.f));
		m_scale.y = 0.90f*m_scale.y + 0.1f*abs(sinf(distort / 300.f));
	}
	else {
		distort = 0.f;
	}
	flared = false;
	chained = false;


}

void Enemy::move(vec2 off)
{
	m_position.x += off.x;
	m_position.y += off.y;

}

void Enemy::setRun(int scalar) {
	if (sleep_time <= 0.f) {
		if (!dragon || fly) {
			vec2 force = vec2({ -scalar * (step + speed_add), 0.f });
			physic.add_Force(force);
			forces_to_delete.emplace_back(force);
		}
		else scalar > 0 ? physic.face_right = false : physic.face_right = true;
	}
}

void Enemy::setJump() {
	if (physic.face_right) setRun(-1);
	else setRun(1);
	/*if (physic.ground) {
		physic.add_Force(vec2({ 0, -jump_speed }));
		forces_to_delete.emplace_back(vec2({ 0, -jump_speed }));
	}*/
	if (physic.ground) {
		physic.jump_force = vec2({ 0, -0.6f*jump_speed });
	}
}



vec2 Enemy::get_position()const
{
	return m_position;
}

void Enemy::set_position(vec2 position)
{
	m_position = position;
}

// Returns the local bounding coordinates scaled by the current size of the enemy
vec2 Enemy::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * 0.4f*enemy_texture.width, std::fabs(m_scale.y) * 0.4f*enemy_texture.height };
}

void Enemy::attack_player(bool att) {
	if (animation_done && sleep_time <= 0.f) {
		int ac = 10;
		if (dragon) ac = 33;
		attack = att;
		action_count = 0;
		if (!dragon && !boss) {
			if (enemy_sound_countdown == 0) {
				Mix_PlayChannel(-1, m_enemy_attack_sound, 0);
				enemy_sound_countdown = 40;
			}
			
		}
		/*else if (boss) {
			if (wolf_sound_countdown == 0) {
				Mix_PlayChannel(-1, m_enemy_wolf_bite, 0);
				wolf_sound_countdown = 60;
			}
			else wolf_sound_countdown--;
		}*/
	}
}

void Enemy::attacked(int damage) {
	hp -= damage;
	healthpoint->current_health_level = hp;
}

int Enemy::get_hp() const{
	return hp;
}

void Enemy::set_height_limit(float h)
{
	
		ground_height = h;
		OriginalHeight = h;
	
}

void Enemy::check_ground()
{

	for (b2ContactEdge* ce = rigidbody->GetContactList(); ce; ce = ce->next)
	{

		void* bodyUserData = ce->other->GetUserData();
		if (bodyUserData) {
			int Bentitytype = static_cast<Renderable*>(bodyUserData)->getEntityType();

			if (Bentitytype == ET_ground && ce->contact->IsTouching())
			{

				b2WorldManifold worldManifold;
				ce->contact->GetWorldManifold(&worldManifold);

				//float normalLength = 0.1f;
				//b2Vec2 normalStart = worldManifold.points[0] - normalLength * worldManifold.normal;
				//b2Vec2 normalEnd = worldManifold.points[0] + normalLength * worldcdManifold.normal;
				int fixture1 = (static_cast<Renderable*> (ce->contact->GetFixtureA()->GetBody()->GetUserData()))->getEntityType();
				int fixture2 = (static_cast<Renderable*> (ce->contact->GetFixtureB()->GetBody()->GetUserData()))->getEntityType();

				b2Vec2 normal = worldManifold.normal;  // poiting from fixture 1 to fixture 2

				printf("%4.2f %4.2f \n", normal.x, normal.y);

				if (fixture1 == ET_ground) {

					if (normal.y == 1) {
						physic.ground = true;
						ground_height = (static_cast<Ground*>(bodyUserData))->Gstartpoint.y * b2topixel;

						if (onCube == false) {
							physic.motion.y = 0;
							onCube = true;
						}

					}
					else if (normal.y == -1) {

						printf("from bottom up");
						physic.motion.y = 1;


					}
					else {

						printf("from side");

					}
				}
				else {

					if (normal.y == -1) {
						physic.ground = true;
						ground_height = (static_cast<Ground*>(bodyUserData))->Gstartpoint.y * b2topixel;

						if (onCube == false) {
							physic.motion.y = 0;
							onCube = true;
						}



					}
					else if (normal.y == 1) {

						printf("from bottom up");
						physic.motion.y = 1;

					}
					else {

						printf("from side");
					}

				}


				return;
			}
		}

		if (ce->next == NULL) {

			/*
			if ( 6 > run_speed >= 3) { run_speed = run_speed * 2; };
			swampContacting = false; }*/

		}
	}


	ground_height = OriginalHeight;
	onCube = false;
	//physic.ground = false;

	groundContacting = 0;
}

void Enemy::check_canAttack() {
	float limit = 60.f;
	if (dragon) limit = 1000.f;
	if (abs(player->get_position().x - m_position.x) <= limit  && abs(player->get_position().y - m_position.y) <= limit && sleep_time <= 0.f) {
		canAttack = true;
	}
	else {
		canAttack = false;
	}
}
void Enemy::check_dead() {
	if (hp <= 0) {
		dead = true;
	}
}

void Enemy::check_animation_done() {
	int ac = 20;
	if (dragon) ac = 66;
	if (action_count == ac && attack) {
		action_count = 0;
		animation_done = true;
	}
	if (action_count == ac / 2 && attack) {
		attack = false;
	}
}


void Enemy::check_touch_fire() {
	if (dragon) touch_fire = false;
	for (auto& fire : *fires) {
		if (abs(fire.get_position().x - m_position.x) <= 80.f){
			touch_fire = true;
			break;
		}
		else
		{
			touch_fire = false;
		}
	}
}

void Enemy::check_healthy() {
	if (hp <= 30) {
		healthy = true;
		if (dragon) {
			if(!fly) m_position.y -= 400;
			fly = true;
			physic.rigid_body = false;
		}
	}
	else
	{
		healthy = false;
	}
}

void Enemy::check_range() {
	float a = 650.f;
	float b = 1000.f;
	if (dragon) {
		a = 1800.f;
		b = 2000.f;
	}
	if (abs(player->get_position().x - m_position.x) <= a) {
		range = true;
	}
	else if (abs(player->get_position().x - m_position.x) >= b) {
		range = false;
	}
}

void Enemy::check_left() {
	if (player->get_position().x <= m_position.x) {
		left = true;
	}
	else {
		left = false;
	}
}

void Enemy::setRage() {
	if (hp <= 25)
		rage = true;
}

sEnemy * Enemy::getInfo()
{
	sEnemy* e1 = new sEnemy;
	e1->action_count = action_count;
	e1->animation_done = animation_done;
	e1->attack = attack;
	e1->boss = boss;
	e1->canAttack = canAttack;
	e1->cd = cd;
	e1->count = count;
	e1->dead = dead;
	e1->down = down;
	e1->exp = exp;
	e1->flareCountDown = flareCountDown;
	e1->flared = flared;
	//e1->forces_to_delete = forces_to_delete;
	e1->healthy = healthy;
	e1->hp = hp;
	e1->jump = jump;
	e1->jump_speed = jump_speed;
	e1->last_position = last_position;
	e1->left = left;
	e1->max_step = max_step;
	e1->m_position = m_position;
	e1->m_rotation = m_rotation;
	e1->m_scale = m_scale;
	e1->rage = rage;
	e1->range = range;
	e1->run = run;
	e1->run_tex = run_tex;
	e1->speed_add = speed_add;
	e1->start_time = start_time;
	e1->step = step;
	e1->tex_num = tex_num;
	e1->touch_fire = touch_fire;
	e1->up = up;
	e1->wcs = wcs;
	e1->type = type;
	e1->dragon = dragon;
	e1->fly = fly;
	e1->ground_height = ground_height;
	return e1;
}

void Enemy::setInfo(sEnemy * e1)
{
	action_count = e1->action_count;
	animation_done = e1->animation_done;
	attack = e1->attack;
	boss = e1->boss;
	canAttack = e1->canAttack;
	cd = e1->cd;
	count = e1->count;
	dead = e1->dead;
	down = e1->down;
	exp = e1->exp;
	flareCountDown = e1->flareCountDown;
	flared = e1->flared;
	//forces_to_delete = e1->forces_to_delete;
	healthy = e1->healthy;
	hp = e1->hp;
	jump = e1->jump;
	jump_speed = e1->jump_speed;
	last_position = e1->last_position;
	left = e1->left;
	max_step = e1->max_step;
	m_position = e1->m_position;
	m_rotation = e1->m_rotation;
	m_scale = e1->m_scale;
	rage = e1->rage;
	range = e1->range;
	run = e1->run;
	run_tex = e1->run_tex;
	speed_add = e1->speed_add;
	start_time = e1->start_time;
	step = e1->step;
	tex_num = e1->tex_num;
	touch_fire = e1->touch_fire;
	up = e1->up;
	wcs = e1->wcs;
	type = e1->type;
	dragon = e1->dragon;
	fly = e1->fly;
	ground_height = e1->ground_height;

	b2Vec2 b2position = b2Vec2((float32)(m_position.x * pixeltob2), (float32)(m_position.y* pixeltob2));
	if (rigidbody) { rigidbody->SetTransform(b2position, 0); }

}