#include "DecisionTree.hpp"



State Node::judge(Enemy* enemy) {
	if (state != State::condition) {
		return state;
	}
	else if (foo(enemy)) {
		return yes->judge(enemy);
	}
	else
	{
		return no->judge(enemy);
	}
}

namespace {
	bool canAttack(Enemy* enemy) {
		return enemy->canAttack;
	}

	bool healthy(Enemy* enemy) {
		return enemy->healthy;
	}

	bool notraged(Enemy* enemy) {
		return !enemy->rage;
	}

	bool isCd(Enemy* enemy) {
		return (enemy->cd <= 0.0);
	}


	bool isleft(Enemy* enemy) {
		return enemy->left;
	}


	bool dead(Enemy * enemy)
	{
		return enemy->dead;
	}

	bool animate_done(Enemy * enemy)
	{
		return enemy->animation_done;
	}

	bool on_fire(Enemy * enemy)
	{
		return enemy->touch_fire;
	}

	bool on_range(Enemy * enemy)
	{
		return enemy->range;
	}

	bool is_boss(Enemy * enemy)
	{
		return enemy->boss;
	}

}

bool DecisionTree::init()
{
	struct Node* Dead = new Node;
	struct Node* Die  = new Node;
	struct Node* Animated = new Node;
	struct Node* End = new Node;
	struct Node* CollideFire = new Node;
	struct Node* Jump = new Node;
	struct Node* Withinrange = new Node;
	struct Node* IsBoss = new Node;
	struct Node* IDLE = new Node;
	
	struct Node* Healthy = new Node;
	struct Node* CanBeAttack = new Node;
	struct Node* NotRaged = new Node;
	struct Node* IsCd = new Node;
	struct Node* Raged = new Node;
	struct Node* Attack = new Node;
	struct Node* isLeft = new Node;
	struct Node* Move_Left = new Node;
	struct Node* Move_Right = new Node;

	Move_Left->state = State::left;
	Move_Left->foo = NULL;
	Move_Left->yes = NULL;
	Move_Left->no = NULL;
	Move_Right->state = State::right;
	Move_Right->foo = NULL;
	Move_Right->yes = NULL;
	Move_Right->no = NULL;
	End->state = State::idle;
	End->foo = NULL;
	End->yes = NULL;
	End->no = NULL;

	Die->state = State::die;
	Die->foo = NULL;
	Die->yes = NULL;
	Die->no = NULL;

	Jump->state = State::jump;
	Jump->foo = NULL;
	Jump->yes = NULL;
	Jump->no = NULL;

	IDLE->state = State::idle;
	IDLE->foo = NULL;
	IDLE->yes = NULL;
	IDLE->no = NULL;

	Attack->state = State::attack;
	Attack->foo = NULL;
	Attack->yes = NULL;
	Attack->no = NULL;

	Raged->state = State::enrage;
	Raged->foo = NULL;
	Raged->yes = NULL;
	Raged->no = NULL;

	isLeft->state = State::condition;
	isLeft->yes = Move_Left;
	isLeft->no = Move_Right;
	isLeft->foo = &isleft;

	IsCd->state = State::condition;
	IsCd->yes = IDLE;
	IsCd->no = Attack;
	IsCd->foo = &isCd;

	CanBeAttack->state = State::condition;
	CanBeAttack->yes = IsCd;
	CanBeAttack->no = isLeft;
	CanBeAttack->foo = &canAttack;

	NotRaged->state = State::condition;
	NotRaged->yes = Raged;
	NotRaged->no = CanBeAttack;
	NotRaged->foo = &notraged;

	Healthy->state = State::condition;
	Healthy->yes = NotRaged;
	Healthy->no = CanBeAttack;
	Healthy->foo = &healthy;

	IsBoss->yes = Healthy;
	IsBoss->no = CanBeAttack;
	IsBoss->state = State::condition;
	IsBoss->foo = &is_boss;

	Withinrange->yes = IsBoss;
	Withinrange->no = IDLE;
	Withinrange->state = State::condition;
	Withinrange->foo = &on_range;

	CollideFire->yes = Jump;
	CollideFire->no = Withinrange;
	CollideFire->state = State::condition;
	CollideFire->foo = &on_fire;


	Animated->yes = CollideFire;
	Animated->no = End;
    Animated->state = State::condition;
	Animated->foo = &animate_done;

	Dead->yes = Die;
	Dead->no = Animated;
	Dead->state = State::condition;
	Dead->foo = &dead;

	

	
	start.push_back(Dead);
	start.push_back(Die);
	start.push_back(Animated);
	start.push_back(End);
	start.push_back(CollideFire);
	start.push_back(Jump);
	start.push_back(Withinrange);
	start.push_back(IsBoss);
	start.push_back(IDLE);
	start.push_back(Healthy);
	start.push_back(CanBeAttack);
	start.push_back(NotRaged);
	start.push_back(IsCd);
	start.push_back(Raged);
	start.push_back(Attack);
	start.push_back(isLeft);
	start.push_back(Move_Left);
	start.push_back(Move_Right);
	
	
	return true;
}

void DecisionTree::destroy() {
	for (auto& node : start) {
    deleteTree(node);
	}
	start.clear();
}

void DecisionTree::deleteTree(Node* root)
{
	if (root != NULL) {
		delete root;
		//root = NULL;
		//deleteTree(root->yes);
		//deleteTree(root->no);
	}
}






