#pragma once
#include "GameObject.hpp"
#include <vector>

struct Physics
{
	std::vector<vec2> forces;
	bool rigid_body;
	bool ground;
	bool face_right;
	vec2 motion;
	vec2 jump_force;   // de-activated once not grounded
	vec2 support_force;  // when grounded, support force is the normal of the plane equation
	vec2 one_time_force;
	vec2 gravity;
	float air_time;
	float orb_jump;

	bool init(bool rigidbody);
	void add_Force(vec2 force);
	void delete_Force(vec2 force);
	vec2 cal_Force(float friction);
	vec2 cal_direction();
	float cal_magnitude();
	void update(float ms);
	void reset_gravity();
};
