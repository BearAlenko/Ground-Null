#include "Emitter.hpp"

Emitter::Emitter() {
	MinimumRadius = -80.f;
	MaximumRadius = 80.f;

	MinSpeed = 0.4f;
	MaxSpeed = 0.6f;

	MinLifetime = 2000.f;
	MaxLifetime = 2400.f;

	Origin = { 0.f, 0.f };
}

void Emitter::EmitParticle(Particle& particle) {
	double pi = acos(-1);
	float radius = RandRange(MinimumRadius, MaximumRadius);
	float speed = RandRange(MinSpeed, MaxSpeed);
	float lifetime = RandRange(MinLifetime, MaxLifetime);

	float X = cosf(particle.init_direction);
	float Y = sinf(particle.init_direction);

	vec2 vector = { X, Y };
	float diff_x = cosf((radius / 180) * pi);  // diff_x*passed time = acceleration of x.
	float diff_y = sinf((radius / 180) * pi);
	particle.m_fRotate = (radius / 180) * pi + particle.init_direction;
	particle.m_Position = Origin;
	particle.m_Velocity = vec2{ (X+diff_x)*speed, (Y+diff_y)*speed };
	particle.acceleration.x = 2*diff_x*speed/lifetime;  // *2 for more visualized acceleration
	particle.acceleration.y = 2*diff_y * speed / lifetime;

	particle.m_LifeTime = lifetime;
	particle.m_consumedTime = 0;
}


