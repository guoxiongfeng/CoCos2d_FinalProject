#pragma once
#include "cocos2d.h"
using namespace cocos2d;

class TowerFactory :public cocos2d::Ref {
public:
	//��ȡ��������
	static TowerFactory* getInstance();
	//����һ�������������洢�������й���
	Sprite* createTower();
	void Attack();
	Sprite * createBullet(Vec2 currentPos, Vec2 dstPos, double speed);
	//�Ƴ�����
	void removeTower(Sprite*);


	void removeAllTower();

	//bool DecreaseHP(Sprite *);
	float attackRadius;
	Vector<Sprite*> tower;
private:
	TowerFactory();
	
	
	static TowerFactory* factory;
};