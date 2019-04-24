#pragma once
#include "GameObject.hpp"
#include "Player.hpp"

class Quest : public Renderable {
public:
	static Texture textures[8];
	bool init(Player* p, bool (*foo)(Player* p), void (*after_completed)(Player* p));
	void destroy();
	void update(float ms);
	void draw(const mat3& projection, mat3& view_2d)override;
	void complete();
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	//sQuest* getInfo();
	//void setInfo(sQuest* q1);
	bool completed = false;
	int texnum = 0;
private:
	Player* player;
	bool (*check)(Player* p) = NULL;
	void(*reward)(Player* p) = NULL;
};

class NewQuest : public Renderable {
public:
	static Texture texture[6];
	bool init();
	void update(float ms);
	void destroy();
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void draw(const mat3& projection, mat3& view_2d)override;
	//sNewQuest* getInfo();
	//void setInfo(sNewQuest* n1);
	bool show = true;
private:

	float start_time = 0.f;
	int tex_num = 0;
};


class QuestManager : public Renderable {
public:
	static Texture texture;
	bool init(Player* player);
	void update(float ms);
	void destroy();
	int getEntityType();
	void handleCollision(Renderable* r1, Renderable* r2, int r1EntityType, int r2EntityType);
	void draw(const mat3& projection, mat3& view_2d)override;
	void insert_quest();
	sQuestManager* getInfo();
	void setInfo(sQuestManager* qm);
	int current = 0;
	bool inserted1 = false;
	bool enlarge = false;
	bool get_empty();

	std::vector<Quest> quests;
	Quest emptyQuest;
private:
	Player* p;
	NewQuest nq;
};

