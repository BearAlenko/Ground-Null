#include "SaveLoad.hpp"

void SaveLoad::save(sPlayer* p1, std::vector<sEnemy*> es, std::vector<sGoldenOrb*> go, std::vector<sCross*> sc, int game_level, sQuestManager* qm, std::vector<sItem*> it, std::vector<sChest*> cc, std::vector<sPillar*> sp)

{
	FILE *SaveFile;

	char FileName[] = PROJECT_SOURCE_DIR "data/save/save1.sav";
	SaveFile = fopen(FileName, "wb"); // opens file for writing and in binary mode


	if (SaveFile == NULL)

	{
		// Error while creating file
		return;

	}

	// save player
	const int playerNo = 1;
	sPlayer Players[1];
	Players[0] = *p1;
	fwrite(&playerNo, sizeof(int), 1, SaveFile);
	fwrite(Players, sizeof(sPlayer), playerNo, SaveFile);

	// save enemy
	int enemyNo = es.size();
	sEnemy* Enemys = new sEnemy[enemyNo];
	for (int i = 0; i < enemyNo; i++) {
		Enemys[i] = *es[i];
	}
	fwrite(&enemyNo, sizeof(int), 1, SaveFile);
	fwrite(Enemys, sizeof(sEnemy), enemyNo, SaveFile);

	// save goldenorb
	int goldenNo = go.size();
	sGoldenOrb* goldenOrb = new sGoldenOrb[goldenNo];
	for (int i = 0; i < goldenNo; i++) {
		goldenOrb[i] = *go[i];
	}
	fwrite(&goldenNo, sizeof(int), 1, SaveFile);
	fwrite(goldenOrb, sizeof(sGoldenOrb), goldenNo, SaveFile);

	// save cross
	int crossNo = sc.size();
	sCross* cross = new sCross[crossNo];
	for (int i = 0; i < crossNo; i++) {
		cross[i] = *sc[i];
	}
	fwrite(&crossNo, sizeof(int), 1, SaveFile);
	fwrite(cross, sizeof(sCross), crossNo, SaveFile);

	// save game level
	int game_lv = game_level;
	fwrite(&game_lv, sizeof(int), 1, SaveFile);

	// save quest

	sQuestManager questM[1];
	questM[0] = *qm;
	fwrite(questM, sizeof(sQuestManager), 1, SaveFile);

	// save item
	int itemNo = it.size();
	sItem* item = new sItem[itemNo];
	for (int i = 0; i < itemNo; i++) {
		item[i] = *it[i];
	}
	fwrite(&itemNo, sizeof(int), 1, SaveFile);
	fwrite(item, sizeof(sItem), itemNo, SaveFile);

	// save chest
	int chestNo = cc.size();
	sChest* c = new sChest[chestNo];
	for (int i = 0; i < chestNo; i++) {
		c[i] = *cc[i];
	}
	fwrite(&chestNo, sizeof(int), 1, SaveFile);
	fwrite(c, sizeof(sChest), chestNo, SaveFile);

	// save pillar
	int pillarNo = sp.size();
	sPillar* p = new sPillar[pillarNo];
	for (int i = 0; i < pillarNo; i++) {
		p[i] = *sp[i];
	}
	fwrite(&pillarNo, sizeof(int), 1, SaveFile);
	fwrite(p, sizeof(sPillar), pillarNo, SaveFile);

	fclose(SaveFile);

	delete[] Enemys;
	delete[] goldenOrb;
	delete[] cross;
	delete[] item;
	delete[] c;
	delete[] p;
	//delete[] questM;

	
}

void SaveLoad::load(Player* p1, sEnemy* ee, int* enemyNo, sGoldenOrb* gorb, int* orbNo, sCross* sCro, int* croNo, int* game_level, QuestManager* qm, sItem* it, int* itemNo, sChest* cc, int* chestNo, sPillar* sp, int* pillarNo)
{
	FILE *LoadFile;
	char FileName[] = PROJECT_SOURCE_DIR "data/save/save1.sav";
	LoadFile = fopen(FileName, "rb");

	if (LoadFile == NULL)

	{
		// Error while creating file
		return;

	}

	// load player
	int playerNo1;
	fread(&playerNo1, sizeof(int), 1, LoadFile);
	sPlayer Players2[1];
	fread(Players2, sizeof(sPlayer), playerNo1, LoadFile);
	p1->setInfo(&Players2[0]);

	// load enemy
	int eNo;
	fread(&eNo, sizeof(int), 1, LoadFile);
	sEnemy* Enemyss = new sEnemy[eNo];
	fread(Enemyss, sizeof(sEnemy), eNo, LoadFile);
	*enemyNo = eNo;
	for (int i = 0; i < eNo; i++) {
		ee[i] = Enemyss[i];
	}
	//ee = Enemys;

	// load goldenorb
	int goldenNo;
	fread(&goldenNo, sizeof(int), 1, LoadFile);
	sGoldenOrb* goo = new sGoldenOrb[goldenNo];
	//gorb = new sGoldenOrb[goldenNo];
	fread(goo, sizeof(sGoldenOrb), goldenNo, LoadFile);
	*orbNo = goldenNo;
	
	for (int i = 0; i < goldenNo; i++) {
		gorb[i] = goo[i];
	}

	// load cross
	int crossNo;
	fread(&crossNo, sizeof(int), 1, LoadFile);
	sCross* sc = new sCross[crossNo];
	//sCro = new sCross[crossNo];
	fread(sc, sizeof(sCross), crossNo, LoadFile);
	*croNo = crossNo;
	
	for (int i = 0; i < crossNo; i++) {
		sCro[i] = sc[i];
	}

	// load game level
	int game_lv;
	fread(&game_lv, sizeof(int), 1, LoadFile);
	*game_level = game_lv;

	// load quest manager
	sQuestManager sqm[1];
	fread(sqm, sizeof(sQuestManager), 1, LoadFile);
	qm->setInfo(&sqm[0]);

	// load item
	int itNo;
	fread(&itNo, sizeof(int), 1, LoadFile);
	sItem* sI = new sItem[itNo];
	fread(sI, sizeof(sItem), itNo, LoadFile);
	*itemNo = itNo;
	for (int i = 0; i < itNo; i++) {
		it[i] = sI[i];
		it[i].rotation = 0.f;
	}

	// load chest
	int cNo;
	fread(&cNo, sizeof(int), 1, LoadFile);
	sChest* sC = new sChest[cNo];
	fread(sC, sizeof(sChest), cNo, LoadFile);
	*chestNo = cNo;
	for (int i = 0; i < cNo; i++) {
		cc[i] = sC[i];
	}
	
	// load pillar
	int pNo;
	fread(&pNo, sizeof(int), 1, LoadFile);
	sPillar* sP = new sPillar[pNo];
	fread(sP, sizeof(sPillar), pNo, LoadFile);
	*pillarNo = pNo;
	for (int i = 0; i < pNo; i++) {
		sp[i] = sP[i];
	}

	fclose(LoadFile);

	delete[] Enemyss;
	delete[] goo;
	delete[] sc;
	delete[] sI;
	delete[] sC;
	delete[] sP;
	//delete[] sqm;

}
