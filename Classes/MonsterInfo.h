#pragma once
#include "cocos2d.h"
using namespace cocos2d;
class MonsterInfo {
	public :
		int hp; //当前hp值
		int maxHp; //最大hp；
	    int kind;//根据kind设置速度， 行为之类
		double speed;
		MonsterInfo(int hp, int kind);
		MonsterInfo();
	private:
		MonsterInfo* setParameters();
};