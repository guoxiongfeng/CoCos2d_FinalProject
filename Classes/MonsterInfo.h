#pragma once
#include "cocos2d.h"
using namespace cocos2d;
class MonsterInfo {
	public :
		int hp; //��ǰhpֵ
		int maxHp; //���hp��
	    int kind;//����kind�����ٶȣ� ��Ϊ֮��
		double speed;
		MonsterInfo(int hp, int kind);
		MonsterInfo();
	private:
		MonsterInfo* setParameters();
};