#pragma once
#include "enemy.hpp"
#include <iostream>
#include <vector>


enum State { condition, attack, die, left, right, idle, enrage, jump };
struct Node
{
	Node* yes;
	Node* no;
	State state;
	bool(*foo) (Enemy*);
	State judge(Enemy* enemy);
};

class DecisionTree{

public:

	std::vector<Node*> start;
	bool init();
	void destroy();
	void deleteTree(Node* root);
};



