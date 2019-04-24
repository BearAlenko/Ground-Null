#pragma once
#include <stdio.h>
#include <vector>
#include "player.hpp"
#include "SaveStruct.hpp"
#include "enemy.hpp"
#include "Viking.hpp"
#include "Wolf.hpp"
#include "Skull.hpp"
#include "Knight.hpp"
#include "GoldenOrb.hpp"
#include "Quest.hpp"

class SaveLoad
{
public:
	void save(sPlayer* p1, std::vector<sEnemy*> es, std::vector<sGoldenOrb*> go, std::vector<sCross*> sc, int game_level, sQuestManager* qm, std::vector<sItem*> it, std::vector<sChest*> cc, std::vector<sPillar*> sp);
	void load(Player*, sEnemy* ee, int* enemyNo, sGoldenOrb* gorb, int* orbNo, sCross* sCro, int* croNo, int* game_level, QuestManager* qm, sItem* it, int* itemNo, sChest* cc, int* chestNo, sPillar* sp, int* pillarNo);

};