#pragma once
#include "cocos2d.h"
using namespace cocos2d;

class Factory:public cocos2d::Ref {
public:
	//��ȡ��������
	static Factory* getInstance();
	//����һ��������洢�������й���
	Sprite* createMonster();
	//�������е����й��ﶼ����ɫ�ƶ���ͨ�������������еĹ���ܷ���
	void moveMonster(Vec2 playerPos,float time);
	//�Ƴ�����
	void removeMonster(Sprite*);
	//�ж���ײ
	Sprite* collider(Rect rect);
	void removeAllMonster();
	bool DecreaseHP(Sprite * monster, bool bulletHit);
	//��ʼ������֡����
	void initSpriteFrame();
	Vector<Sprite*> monster;
private:
	Factory();
	
	cocos2d::Vector<SpriteFrame*> monsterDead;
	static Factory* factory;
};