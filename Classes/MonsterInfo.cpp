#include "MonsterInfo.h"

MonsterInfo::MonsterInfo(int hp, int kind = 0)
{
	this->maxHp = hp;
	this->hp = hp;
	this->kind = kind;
	setParameters();
}

MonsterInfo::MonsterInfo()
{
	this->hp = 1;
	this->kind = 0;
	setParameters();
}

MonsterInfo* MonsterInfo::setParameters()
{/*
	switch (kind)
	{
	
	default:
		break;
	}
	*/
	this->speed = 1 + this->kind / 10.0;
	return this;
}


