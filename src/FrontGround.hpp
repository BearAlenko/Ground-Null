#pragma once

#include "GameObject.hpp"


class FrontGround : public Renderable
{
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Renders the frontGround
	void draw(const mat3& projection, mat3& view_2d)override;

	// Set dead time
	void set_player_dead();
	void reset_player_dead_time();
	float get_player_dead_time() const;
	Texture frontGround;
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);

private:
	// When player is alive, the time is set to -1
	
	float m_dead_time;
};
