#include "Collision.hpp"
#include "vector"
#include "algorithm"
#include "world.hpp"






bool Collision::init()
{
	return true;
}


AABB* Collision::Abinit(vec2 p1, vec2 p2, AABBtype type) {
	AABB* a1 = new AABB;
	value*  min = new value;
	min->min = 1;
	min->master = a1;
	min->var = p1;
	a1->min = min;
	a1->type = type;

	value*  max = new value;
	max->min = 0;
	max->master = a1;
	max->var = p2;
	a1->max = max;
	return a1;
};

bool Collision::testAABBOverlap(AABB* a, AABB* b) {
	float d1x = b->min->var.x - a->max->var.x;
	float d1y = b->min->var.y - a->max->var.y;
	float d2x = a->min->var.x - b->max->var.x;
	float d2y = a->min->var.y - b->max->var.y;

	if (d1x > 0.0f || d1y > 0.0f)
		return false;

	if (d2x > 0.0f || d2y > 0.0f)
		return false;

	return true;
};


bool compare(value* a, value* b)
{
	if (a->var.x < b->var.x)
		return true;
	return false;
}



void Collision::SweepAndPrune(std::vector<AABB*> ab, int* countdown)
{
	std::vector<value*> todo;
	std::vector<AABB*> activelist;

	// add values to todolist
	for (int i = 0; i < ab.size(); i++) {
		todo.emplace_back(ab[i]->min);
		todo.emplace_back(ab[i]->max);
	}

	//sort todolist
	sort(todo.begin(), todo.end(), compare);

	//iterate the todo list and add them to the activelist
	for (int i = 0; i < todo.size(); i++) {
		if (todo[i]->min) {
			//check activelist
			activelist.emplace_back(todo[i]->master);
			if (activelist.size() != 1) {
				for (int j = 0; j < activelist.size(); j++) {

					AABB* poly1 = activelist[j];

					for (int k = j + 1; k < activelist.size(); k++) {

						AABB* poly2 = activelist[k];

						if (!poly1->checked && !poly2->checked) {
							if (testAABBOverlap(poly1, poly2)) {

								poly1->checked = true;
								poly2->checked = false;

								if (poly1->flare != NULL) { poly1->checked = false; }
								if (poly1->ground != NULL) { poly1->checked = false; }

								if (poly1->type != cross_a && poly2->type != cross_a) {
									CollisionCatch(poly1, poly2, countdown);

								}

								else if (intersection(poly1->vextex, poly2->vextex)) {
									CollisionCatch(poly1, poly2, countdown);
								}

								else if (insideTest(poly1->vextex, poly2->vextex, poly1->convex, poly2->convex, poly1->indices, poly2->indices)) {
									CollisionCatch(poly1, poly2, countdown);
								}
							}
						}

					}
				}
			}
		}
		else {
			int index = findAB(todo[i], activelist);
			activelist[index]->checked = false;

			if (index >= activelist.size()) {
			
			}
			else {
             
			activelist.erase(activelist.begin() + index);

		}
		}
	}
}

int Collision::findAB(value * min, std::vector<AABB*> activelist)
{
	for (int i = 0; i < activelist.size(); i++) {
		if (min->master == activelist[i]) return i;
	}
	return 0;
}

bool Collision::SegVsSeg(vec2 p1s, vec2 p1e, vec2 p2s, vec2 p2e)
{
	float p0_x = p1s.x, p0_y = p1s.y, p1_x = p1e.x, p1_y = p1e.y;
	float p2_x = p2s.x, p2_y = p2s.y, p3_x = p2e.x, p3_y = p2e.y;

	float s1_x, s1_y, s2_x, s2_y;
	s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
	s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

	float s, t;
	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
	t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{

		return true;
	}

	return false; // No collision
}

bool Collision::LineVsLine(vec2 p1s, vec2 p1e, vec2 p2s, vec2 p2e)
{
	return false;
}

bool Collision::PointOnLine(vec2 p1, vec2 p2s, vec2 p2e)
{
	int y1 = p2s.y;
	int y2 = p2e.y;
	int x1 = p2s.x;
	int x2 = p2e.x;
	int  x = p1.x;
	int  y = p1.y;
	int e = (y2 - y1)*(x - x1) - (x2 - x1)*(y - y1);
	return e;
}

bool Collision::intersection(std::vector<vec2> poly1, std::vector<vec2> poly2)
{

	for (int i = 0; i < poly1.size(); i++) {


		vec2 p1s = poly1[i];
		vec2 p1e;


		if (i == poly1.size() - 1) {
			p1e = poly1[0];
		}
		else {
			p1e = poly1[i + 1];
		}


		for (int j = 0; j < poly2.size(); j++) {

			vec2 p2s = poly2[j];
			vec2 p2e;

			if (j == poly2.size() - 1) {
				p2e = poly1[0];
			}
			else {
				p2e = poly1[i + 1];
			}

			if (SegVsSeg(p1s, p1e, p2s, p2e)) { return true; }
		}

	}

	return false;
}



bool Collision::insideTest(std::vector<vec2> poly1, std::vector<vec2> poly2, bool convex1, bool convex2, std::vector<uint16_t> indices1, std::vector<uint16_t> indices2)
{

	// first calculate if poly1 is out of poly2
	if (convex1) {

		for (int i = 0; i < poly2.size(); i++) {

			vec2 p1 = poly2[i];
			bool rsf;

			for (int j = 0; j < poly1.size(); j++) {


				vec2 p2s = poly1[j];
				vec2 p2e;

				if (j == poly1.size() - 1) {
					p2e = poly1[0];
				}
				else {
					p2e = poly1[j + 1];
				}


				if (j == 0) { rsf = PointOnLine(p1, p2s, p2e); }

				if (PointOnLine(p1, p2s, p2e) != rsf) {
					break;
				}

			}

			if (i == poly2.size() - 1) {

				return true;
			}
		}


	}
	/*else {


	}*/



	//then calculate if poly1 is inside poly2
	if (convex2) {
		for (int i = 0; i < poly1.size(); i++) {

			vec2 p1 = poly1[i];
			bool rsf;

			for (int j = 0; j < poly2.size(); j++) {


				vec2 p2s = poly2[j];
				vec2 p2e;

				if (j == poly2.size() - 1) {
					p2e = poly2[0];
				}
				else {
					p2e = poly2[j + 1];
				}


				if (j == 0) { rsf = PointOnLine(p1, p2s, p2e); }

				if (PointOnLine(p1, p2s, p2e) != rsf) {
					break;
				}

			}

			if (i == poly1.size() - 1) {

				return true;
			}
		}
	}
	/*else {

	}*/


	return false;

}

void Collision::CollisionCatch(AABB * e1, AABB * e2, int* countdown)
{



	switch (e1->type) {
	case player_a: {

		if (e2->type == enemy_a) {

			    /*
				if (*countdown <= 0) {
					*countdown = 70;
					if (e2->enemy != NULL && e2->enemy->get_hp() > 0 && e1->player->m_is_alive) {
						e1->player->collides_with(*e2->enemy);
					}
				}*/
		}
	
		else if (e2->type == fire_a) {

			/*
			if (*countdown <= 0) {
				*countdown = 70;
				e1->player->collides_with(*e2->fire);	
			}*/
		}

		else if (e2->type == mace_a) {

			/*
			if (*countdown <= 0) {
				*countdown = 70;
				e1->player->collides_with(*e2->mace);
			}*/
		}

		else if (e2->type == venom_a) {
			/*
			if(e2->venom->state_flag == true){
				if (*countdown <= 0) {
					*countdown = 70;
					e1->player->collides_with(*e2->venom);
				}
			}*/
		}

		else if (e2->type == ground_a) {

			//In this case, ground should at the left side of player


			//Get statistics for case
			int x = e1->player->get_position().x - e2->ground->get_position().x; //should be positive
			int y = e1->player->get_position().y - e2->ground->get_position().y;

			int dx = abs(x);
			int dy = abs(y);

			vec2 norm = { x,y };


			// first test if player is inside the real block, AABB is a little larger the  the real block

			//if (e1->player->checking == false) {
			if (e2->ground->checked == false) {

				if (dx < 75 &&  /* -160 < y && y < 0 */ dy < 135 && e2->ground->checked == false) {

					vec2 prenorm = { 0,0 };

					if (y < 0 && y > -135) {
						prenorm = RemoveOut(e1, e2);
					}

					if (y < 70 && y > 0) {
						prenorm = RemoveOut(e1, e2);
					}


					// if it is inside the block , remove it out , then based on the the normol, decides actions

					// if it is from the up side of block, in this case, we should let the player stand on this cube
					if (prenorm.y == -1) {

						if (e1->player->onCube == false) { e1->player->physic.motion.y = 0; }
						e1->player->physic.ground = true;
						e1->player->onCube = true;
						e2->ground->checked = true;
						e1->player->needcheck = false;
						e2->ground->playeron = true;
					}


					// if it is from the other side of block, we should make it back, not not make it on the cube
					else if (prenorm.y == 1) {

						// if player is at the bottom of block and is jumping up
						if (prenorm.y == 1) {
							e1->player->physic.motion.y = 100;
						}

						// Has already moved it out because of function RemoveOut

					}
					else {

					}

				}

				// if player is outside of or on the block, decides actions based on relative positions 

				else {

					if (e2->ground->playeron && e1->player->needcheck == false) {
						e1->player->onCube = false;
						e1->player->needcheck = true;
						e2->ground->playeron = false;
					}
				}
				e2->ground->checked = true;
			}
		}
		/*
		else if (e2->type == jump_orb_a) {
		e2->orb->collides_with(*e1->player);
		}*/

		else if (e2->type == fireball_a) {

		/*
		
			if (e2->fireball->fireball_status()) {
				if (*countdown <= 0) {
					*countdown = 70;
					e1->player->collides_with(*e2->fireball);
				}
			}*/
		}

		else if (e2->type == cross_a) {
		/*
		e2->cross->collidedP(*e1->player);
		*/
		}

		else if (e2->type == gold_orb_a) {
		   /*
			if (e1->player->m_is_alive) {

				if (e2->gold->erase_flag == false) {

					e1->player->add_keys();
					e2->gold->erase_flag = true;
				}
			}*/
		}
		else if (e2->type == stone_a) {

		     /*
			if (*countdown <= 0) {

				if (e2->sstone->collides_with(*e1->player)) {
					*countdown = 70;
				}
			}*/
        }

		else if (e2->type == dragonfire_a) {

		/*
			if(e2->df->shouldErase != true && e1->player->m_is_alive){

				if (*countdown <= 0) {
					*countdown = 70;
					e2->df->collides_with(*e1->player);
				}
			}*/
        }

		break;
	}

	case enemy_a: {
		if (e2->type == player_a) {

			/*

			if (e2->player->m_is_alive) {

 				if (e1->enemy != NULL && e1->enemy->get_hp() > 0) {

					if (*countdown <= 0) {
						*countdown = 70;
						e2->player->collides_with(*e1->enemy);
					}
				}
			}*/
		}
		else if (e2->type == fire_a) {

		}
		else if (e2->type == attack_a) {

			/*
			if (e2->attack != NULL && e2->attack->get_duration() > 0 && !e2->attack->hit && e1->enemy != NULL && e1->enemy->get_hp() > 0) {
				e2->attack->shouldErase = true;
				e2->attack->hit = true;
				e2->attack->enemyposition = e1->enemy->get_position();
				e2->attack->collides_with(*e1->enemy);
			}*/

		}
		else if (e2->type == shield_a) {
			/*
			if (e2->shield != NULL && e2->shield->get_duration() > 0 && !e2->shield->hit && e1->enemy != NULL && e1->enemy->get_hp() > 0) {
				e2->shield->shouldErase = true;
				e2->shield->hit = true;
				e2->shield->collides_with(*e1->enemy);
			}*/

		}

		else if (e2->type == flare_a) {
	
			/*
			if (e2->flare != NULL && e2->flare->get_duration() > 0 && e1->enemy != NULL && e1->enemy->get_hp() > 0 && !e1->enemy->flared) {
				if (e2->flare->collides_with(*e1->enemy)) { e1->enemy->flared = true; };
			}
			*/
		
		}

		else if (e2->type == ground_a) {
			int x = e1->enemy->get_position().x - e2->ground->get_position().x; //should be positive
			int y = e1->enemy->get_position().y - e2->ground->get_position().y;
			int dx = abs(x);
			int dy = abs(y);

			vec2 norm = { x,y };

			if (e1->enemy->echecked == false ) {

				if (dx < 85 &&  dy < 105) {

					vec2 prenorm = { 0,0 };

					if (y < 0 && y > -105) {
						prenorm = RemoveOut(e1, e2);
					}

					if (y < 70 && y > 0) {
						prenorm = RemoveOut(e1, e2);
					}

					if (prenorm.y == -1) {
						if (e1->enemy->onCube == false) { e1->enemy->physic.motion.y = 0; }
						e1->enemy->physic.ground = true;
						e1->enemy->onCube = true;
						e1->enemy->echecked = true;
						e1->enemy->enemyon = true;
						e1->enemy->needcheck = false;
					}

					else if (prenorm.y == 1) {

						if (prenorm.y == 1) {

							e1->enemy->physic.motion.y = 5;

						}

					}
					else {
						e1->enemy->physic.one_time_force = vec2({ -e1->enemy->physic.motion.x, 0.f });


					}

				}

				else {

					if (e1->enemy->enemyon && e1->enemy->needcheck == false) {
						e1->enemy->onCube = false;
						e1->enemy->needcheck = true;
						e1->enemy->enemyon = false;
					}
				}

				e1->enemy->echecked = true;
			}
		
		}
		break;
	}
	case shield_a: {

		/*
		
		if (e2->type == enemy_a) {
			if (e1->shield != NULL && e1->shield->get_duration() > 0 && !e1->shield->hit && e2->enemy != NULL && e2->enemy->get_hp() > 0) {
					e1->shield->shouldErase = true;
					e1->shield->hit = true;
					e1->shield->collides_with(*e2->enemy);
			}
		}
		else*/ if (e2->type == fire_a) {}
		break;
	}
	case attack_a: {
		/*
		if (e2->type == enemy_a) {
			
			if (e1->attack != NULL && e1->attack->get_duration() > 0 && !e1->attack->hit && e2->enemy != NULL && e2->enemy->get_hp() >0) {
				e1->attack->shouldErase = true;
				e1->attack->hit = true;
				e1->attack->enemyposition = e2->enemy->get_position();
				e1->attack->collides_with(*e2->enemy);
			}
			
		}*/
		break;
		
	}
	case fire_a: {
		/*
		if (e2->type == enemy_a) {
		}
		else if (e2->type == player_a) {
			if (*countdown <= 0) {
				*countdown = 70;
				e2->player->collides_with(*e1->fire);
			}
		}*/
		break;
	}
	case flare_a: {
		/*
		if (e2->type == enemy_a) {
			
			if (e1->flare != NULL && e1->flare->get_duration() > 0 && e2->enemy != NULL && e2->enemy->get_hp() > 0 && !e2->enemy->flared) {

				if (e1->flare->collides_with(*e2->enemy)) { e2->enemy->flared = true; }	
			}
		}*/
		break;
	}

	case cross_a: {
		/*
		if (e2->type == player_a) {
			e1->cross->collidedP(*e2->player);
		}*/
		break;
	}
	case gold_orb_a: {
		/*
		if (e2->type == player_a) {
			if (e2->player->m_is_alive) {
				if (e1->gold->erase_flag == false) {
					e2->player->add_keys();
					e1->gold->erase_flag = true;
				}
			}
		}*/
		break;
	}

	case fireball_a: {
		/*
		if (e2->type == player_a) {
			if (e1->fireball->fireball_status()) {
					if (e1->fireball != NULL) {
						if (*countdown <= 0) {
							*countdown = 70;
							e2->player->collides_with(*e1->fireball);
						}
					}			
			}
		}*/
		break;
	}
	case mace_a: {
		/*
		if (e2->type == player_a) {
			if (*countdown <= 0) {
				*countdown = 70;
				e2->player->collides_with(*e1->mace);
			}
		}*/
		break;
	}
	case venom_a: {
		if (e2->type == player_a) {
			/*
			if(e1->venom->state_flag == true){
				if (*countdown <= 0) {
					*countdown = 70;
					e2->player->collides_with(*e1->venom);
				}
			}*/	
		}
		break;
	}

	case ground_a: {
		if (e2->type == player_a) {


			//In this case, ground should at the left side of player


			//Get statistics for case
			int x = e2->player->get_position().x - e1->ground->get_position().x; //should be positive
			int y = e2->player->get_position().y - e1->ground->get_position().y;

			int dx = abs(x);
			int dy = abs(y);

			vec2 norm = { x,y };


			// first test if player is inside the real block, AABB is a little larger the  the real block


			if (e1->ground->checked == false) {

				if (dx < 75 &&  /* -160 < y && y < 0 */ dy < 135 && e1->ground->checked == false) {

					vec2 prenorm = { 0,0 };

					if (y < 0 && y > -135) {
						prenorm = RemoveOut(e2, e1);
					}

					if (y < 70 && y > 0) {
						prenorm = RemoveOut(e2, e1);
					}



					// if it is inside the block , remove it out , then based on the the normol, decides actions

					// if it is from the up side of block, in this case, we should let the player stand on this cube
					if (prenorm.y == -1) {


						if (e2->player->onCube == false) { e2->player->physic.motion.y = 0; }
						e2->player->physic.ground = true;
						e2->player->onCube = true;
						e1->ground->checked = true;
						e1->ground->playeron = true;
						e2->player->needcheck = false;


					}


					// if it is from the other side of block, we should make it back, not not make it on the cube
					else if (prenorm.y == 1) {


						// if player is at the bottom of block and is jumping up
						if (prenorm.y == 1) {


							//if player is on ground, can't move forward
							e2->player->physic.motion.y = 5;

						}

						// Has already moved it out because of function RemoveOut

					}
					else {
						e2->player->physic.one_time_force = vec2({ -e2->player->physic.motion.x, 0.f });


					}

				}

				// if player is outside of or on the block, decides actions based on relative positions 

				else {

					if (e1->ground->playeron && e2->player->needcheck == false) {
						e2->player->onCube = false;
						e2->player->needcheck = true;
						e1->ground->playeron = false;
					}
				}

				e1->ground->checked = true;
			}


		}
		

			else if (e2->type == enemy_a) {
			
			int x = e2->enemy->get_position().x - e1->ground->get_position().x;
			int y = e2->enemy->get_position().y - e1->ground->get_position().y;

			int dx = abs(x);
			int dy = abs(y);

			vec2 norm = { x,y };

			if (e2->enemy->echecked == false) {

				if (dx < 85 && dy < 135) {

					vec2 prenorm = { 0,0 };

					if (y < 0 && y > -135) {
						prenorm = RemoveOut(e2, e1);
					}

					if (y < 70 && y > 0) {
						prenorm = RemoveOut(e2, e1);
					}

					if (prenorm.y == -1) {


						if (e2->enemy->onCube == false) { e2->enemy->physic.motion.y = 0; }
						e2->enemy->physic.ground = true;
						e2->enemy->onCube = true;
						e2->enemy->enemyon = true;
						e2->enemy->needcheck = false;
					}


					else if (prenorm.y == 1) {


						if (prenorm.y == 1) {

							e2->enemy->physic.motion.y = 5;

						}


					}
					else {
						e2->enemy->physic.one_time_force = vec2({ -e2->enemy->physic.motion.x, 0.f });
					}
				}


				else {

					if (e2->enemy->enemyon && e2->enemy->needcheck == false) {
						e2->enemy->onCube = false;
						e2->enemy->needcheck = true;
						e2->enemy->enemyon = false;
					}
				}

				e2->enemy->echecked = false;
			}
			}
			break;
		
	
	}
	case jump_orb_a: {
	
		/*
		if (e2->type == player_a) {
			if (e1->orb->collides_with(*e2->player)) { 
				e1->orb->collided = true; 
			}
		}*/
		break;
	}
	case stone_a: {

		/*
		if (e2->type == player_a) {
			if (*countdown <= 0) {
				if (e1->sstone->collides_with(*e2->player)) {
					*countdown = 70;
				}
			}
		}*/
		break;
	}
	case dragonfire_a: {
	   /*
		if (e2->type == player_a) {
			if (e1->df->shouldErase != true && e2->player->m_is_alive) {
				if (*countdown <= 0) {
					*countdown = 70;
					e1->df->collides_with(*e2->player);
				}
			}
		}*/
		break;
	}
	}
}

void Collision::addvertex(AABB * a1, std::vector<vec2> vertex)
{
	a1->vextex = vertex;
}

void Collision::addverindices(AABB * a1, std::vector<uint16_t> indices)
{
	a1->indices = indices;
}

void Collision::convex(AABB * a1, bool convex)
{
	a1->convex = convex;
}

vec2 Collision::RemoveOut(AABB* player, AABB* enemy)
{


	if (player->type == player_a) {

		int dx = player->player->last_position.x - enemy->ground->get_position().x;
		int dy = player->player->last_position.y - enemy->ground->get_position().y;

		float x;
		vec2 normal = { 0 , 0 };

		if (abs(dx) > abs(dy)) {

			if (dx >= 0) { /*return*/

				player->player->m_position.x = enemy->ground->get_position().x + 90;
				normal = { 1,0 };

			}
			else {

				player->player->m_position.x = enemy->ground->get_position().x - 90;
				normal = { -1,0 };

			}
		}
		else {

			if (dy >= 0) {

				player->player->m_position.y = enemy->ground->get_position().y + 70;
				normal = { 0,1 };

			}
			else {

				player->player->m_position.y = enemy->ground->get_position().y - 130;
				normal = { 0,-1 };
			}
		}

		return normal;
	}
	else if (player->type == enemy_a) {

		int dex = player->enemy->last_position.x - enemy->ground->get_position().x;
		int dey = player->enemy->last_position.y - enemy->ground->get_position().y;
		dey -= 20;

		float x;
		vec2 normal = { 0 , 0 };

		if (abs(dex) > abs(dey)) {

			if (dex >= 0) { /*return*/

				player->enemy->m_position.x = enemy->ground->get_position().x + 90;
				normal = { 1,0 };

			}
			else {

				player->enemy->m_position.x = enemy->ground->get_position().x - 90;
				normal = { -1,0 };

			}
		}
		else {

			if (dey >= 0) {
                
				player->enemy->m_position.y = enemy->ground->get_position().y + 70;
				normal = { 0,1 };

			}
			else {

				player->enemy->m_position.y = enemy->ground->get_position().y - 105;
				normal = { 0,-1 };
			}
		}

		return normal;
	}
}

