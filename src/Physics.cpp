#include "Physics.hpp"

bool Physics::init(bool rigidbody) {
	forces.clear();
	face_right = true;
	ground = true;
	rigid_body = rigidbody;
	motion = vec2 {0.f, 0.f};
	jump_force = vec2{ 0.f, 0.f };
	if (rigidbody) gravity = vec2({ 0.f, Global_gravity });
	support_force = vec2({0.f, -Global_gravity});
	one_time_force = vec2({ 0.f, 0.f });
	air_time = 0.f;
	orb_jump = false;
	return true;
}

void Physics::add_Force(vec2 force) {
	forces.emplace_back(force);
}

void Physics::delete_Force(vec2 force) {
	int count = 0;
	for (auto& f : forces) {
		if (f.x == force.x && f.y == force.y) {
			forces.erase(forces.begin() + count);
			break;
		}
		count++;
	}
}

vec2 Physics::cal_Force(float friction) {
	vec2 total_force = { 0, 0 };
	
	for (auto& f : forces) {
		total_force.x += f.x;
		total_force.y += f.y;
	}
	if (orb_jump<= 0.f) {                   // when orb jumping, no other forces allowed
		motion.x += total_force.x;
		motion.y += total_force.y;
	}
	
	

	////////////// one-time jump force/////////////
	if (!ground) jump_force = vec2({ 0.f, 0.f });  // Can't jump when in the air
	motion.y += jump_force.y;
	motion.x += jump_force.x;
	
	////////////// gravity ///////////////////
	motion.y += gravity.y;
	motion.x += gravity.x;

	///////////// support force ///////////////
	motion.y += support_force.y;
	motion.x += support_force.x;

	//////////// one-time force///////////////
	motion.y += one_time_force.y;
	motion.x += one_time_force.x;
	one_time_force = vec2({0.f,0.f});   // reset when adding finishes

	//////////////// friction /////////////////////////
	if (abs(motion.x) > 0 && orb_jump <= 0.f) {  // no friction when orb jumping
		if (abs(motion.x) >= friction) motion.x -= motion.x / abs(motion.x)*(friction);
		else motion.x = 0;
	}

	/////////////// facing ///////////////////////////
	if (total_force.x > 0 && !face_right) face_right = true;
	if (total_force.x < 0 && face_right) face_right = false;
	return motion;
}

vec2 Physics::cal_direction() {
	vec2 direction = normalize(motion);
	return direction;
}

float Physics::cal_magnitude() {
	vec2 direction = normalize(motion);
	return motion.x / direction.x;
}

void Physics::update(float ms) {
	if (rigid_body) {
		if (!ground) {
			///////////////// When not touching the land, the first jump_force is de-activated. Second jump is available now ///////////////////
			jump_force = vec2({ 0.f, 0.f }); 
			support_force = vec2({0.f, 0.f});
			orb_jump -= ms;
		}
		else orb_jump = 0.f;
		gravity = vec2({ 0.f, Global_gravity });      // keep update if the entity is rigid_body
	}
	else {
		gravity = vec2({ 0.f, 0.f });
	}
}

void Physics::reset_gravity() {
	air_time = 0;
}