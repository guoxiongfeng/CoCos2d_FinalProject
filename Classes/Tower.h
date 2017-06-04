#pragma once
#include "cocos2d.h"
using namespace cocos2d;

class TowerFactory :public cocos2d::Ref {
public:
	//获取单例工厂
	static TowerFactory* getInstance();
	//生成一个防御塔，并存储到容器中管理
	Sprite* createTower();
	void Attack();
	Sprite * createBullet(Vec2 currentPos, Vec2 dstPos, double speed);
	//移除怪物
	void removeTower(Sprite*);


	void removeAllTower();

	//bool DecreaseHP(Sprite *);
	float attackRadius;
	Vector<Sprite*> tower;
private:
	TowerFactory();
	
	
	static TowerFactory* factory;
};