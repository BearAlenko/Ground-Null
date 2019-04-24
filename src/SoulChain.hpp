#pragma once
#include "GameObject.hpp"
#include "Enemy.hpp"
#include "ParticleEffect.hpp"


class SoulChain: public Renderable{
public:
	static Texture texture;
	bool init(Player* player, Enemy* enemy);
	void update(float ms);
	void destroy();
	void draw(const mat3& projection, mat3& view_2d);
	void start();
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	bool in_range = false;
	bool activated = false;
	bool enough_lv = false;
	void check_enough_lv();
	void set_enemy(Enemy* en);
private:
	int count_down;
	ParticleEffect particle_effect;
	Player* p;
	Enemy* e;
	vec2 m_position = {0.f, 0.f};
	float m_rotation = 0.f;
	float m_scale = 0.8f;
};
