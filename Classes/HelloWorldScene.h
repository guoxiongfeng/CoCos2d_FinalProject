#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

	void SlowHeal(float dt);

	void addContactListener();

	bool onConcactBegin(PhysicsContact & contact);

	void TowerAttack(float dt);

	void update(float dt);

	void MoveCallback(Ref * pSender);

	void Jump();

	bool CheckAnimation();

	Sequence * addStatic(Animate * t);

	void Dead();

	void Reviving(float dt);

	void Revive(Vec2 pos);

	void DisablePlayer();

	void Attack();

	void setText();

	void UpdatePlayerLevel();

	void Move(Vec2 direction);
	bool isValid(Vec2 t, Vec2 by);
	//static void OnMoveFinished();
	//bool isAnimateExcuting;
	std::string setLabelText(int k);

	void createMonster(float dt);
	void moveMonster(float dt);

	void InitialScene();

	void setMap();

	void detactKeyboardEvent(float dt);

	void setEvent();

	void AnimationInit();


	void SetProgressBar();

	bool contactWithTower();

	void hitByMonster(float dt);

	void playerDecreaseBlood(int amount);

	void GameOver();

	void replayCallback(Ref * pSender);

	void exitCallback(Ref * pSender);

	void addTouchListener();

	bool onTouchBegan(Touch * touch, Event * event);

	void UpdateValidTower();

	void onKeyPressed(EventKeyboard::KeyCode code, Event * event);

	void onKeyReleased(EventKeyboard::KeyCode code, Event * event);

	void Level_2_Skill();

	void Level_3_Skill();

	void Level3Effect(float dt);

	void Level_4_Skill();

	void Level4Effect(float dt);

	void Level_5_Skill();

	void UpdateColdTime(float dt);

	static bool Level_5_Skill_On;
	static int currentScore;
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
	//void update(float dt) override;
private:
	cocos2d::Sprite* player;
	cocos2d::Sprite* home;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	cocos2d::Label* validTowerNum;
	cocos2d::Label* PlayerLevel;
	cocos2d::Label* reviveTime;
	cocos2d::Sprite* skill_icon[5];
	cocos2d::Sprite* skill_show[4];
	int dtime;
	int attackOfPlayer;
	int resumeFromAttack;
	cocos2d::ProgressTimer* pT;
	bool isPressed[4];
	std::map<EventKeyboard::KeyCode, bool> pressTable;
	int skillColding[4]; //技能剩余冷却时间
	PhysicsWorld* world;
};
#endif // __HELLOWORLD_SCENE_H__

