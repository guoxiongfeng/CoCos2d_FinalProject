#include"Monster.h"
#include "MonsterInfo.h"
#include "HelloWorldScene.h"
#define BloodBar -30000
#define MonsterIsDecreasingHp -30001
#define MONSTER -1
#define MonsterBaseHP 3
USING_NS_CC;

Factory* Factory::factory = NULL;
Factory::Factory() {
	initSpriteFrame();
}
Factory* Factory::getInstance() {
	if (factory == NULL) {
		factory = new Factory();
	}
	return factory;
}
void Factory::initSpriteFrame(){
	auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258-48*i,0,42,42)));
		monsterDead.pushBack(frame);
	}
}

Sprite* Factory::createMonster() {
	Sprite* mons = Sprite::create("Monster.png", CC_RECT_PIXELS_TO_POINTS(Rect(364,0,42,42)));
	mons->setAnchorPoint(Vec2(0.5, 0.5));
	mons->setName("monster");
	
	PhysicsBody* rigidbody = PhysicsBody::createBox(mons->getContentSize());
	rigidbody->setGravityEnable(false);
	rigidbody->setTag(MONSTER);
	mons->addComponent(rigidbody);
	rigidbody->setCategoryBitmask(0xFFFFFFFF);
	rigidbody->setCollisionBitmask(0xFFFFFFFF);
	rigidbody->setContactTestBitmask(0xFFFFFFFF);

	//这是难度的重点。 这函数我随便写的
	mons->setUserData(new MonsterInfo(MonsterBaseHP + HelloWorld::currentScore / 150, HelloWorld::currentScore / 150));

	//设置血条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar 血条需要更好看的素材
	auto pT = ProgressTimer::create(sp);
	pT->setScaleX(10);
	pT->setScaleY(0.4);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition((Vec2((mons->getContentSize().width - 20) / 2  , mons->getContentSize().height / 1.0)));
	pT->setTag(BloodBar);
	mons->addChild(pT, 1);

	monster.pushBack(mons);
	return mons;
}

void Factory::removeMonster(Sprite* sp) {
	Animation * anim = Animation::createWithSpriteFrames(monsterDead, 0.1f);
	Animate * ani = Animate::create(anim);
	Sequence *seq = Sequence::create(ani, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sp)), NULL);
	sp->runAction(seq);
	if (monster.find(sp) != monster.end()) {
		monster.eraseObject(sp);
		HelloWorld::currentScore += 10;
	}
}
//设置monster移动路径是控制难度的关键
//暂时用简单的直线运动吧
void Factory::moveMonster(Vec2 playerPos,float time){
	double speed;
	for (auto it = monster.begin(); it != monster.end(); ++it) {
		Vec2 MonsterPos = (*it)->getPosition();
		//Vec2 direction = playerPos - MonsterPos;
		Vec2 direction = Vec2(-1, 0);
		direction.normalize();
		speed = ((const MonsterInfo * const)(*it)->getUserData())->speed;
		//speed = HelloWorld::currentScore / 300.0 + 1;
		(*it)->runAction(MoveBy::create(time, direction * speed));
	}
}

Sprite* Factory::collider(Rect rect) {
	for (auto it = monster.begin(); it != monster.end(); ++it) {
		Point monsterP = (*it)->getPosition();
		if (rect.containsPoint(monsterP)) {
			return (*it);
		}
	}
	return NULL;
}

void Factory::removeAllMonster() {
	monster.clear();
}

//Added
bool Factory::DecreaseHP(Sprite * monster, bool bulletHit) {
	//正在减血就不能继续减了
	if (monster == nullptr) return false;
	auto bar = (ProgressTimer *)monster->getChildByTag(BloodBar);
	if (bar == nullptr) return false;
	if (bar->getActionByTag(MonsterIsDecreasingHp) != nullptr && !bulletHit) 
		return false;

	MonsterInfo * t = (MonsterInfo *)monster->getUserData();
	t->hp--;
	//这里用不用动画是一个问题。 减血迅速的时候有问题。
	ProgressFromTo * process = nullptr;
	if (bulletHit)
		//bullet击中，每次减1
		process = ProgressFromTo::create(0.02f, bar->getPercentage(), bar->getPercentage() -  100 / t->maxHp);
	else 
		//人击中， 当然要比较厉害。 每次固定15% 伤害。（而且我这种写法是范围群伤。
		process = ProgressFromTo::create(0.02f, bar->getPercentage(), bar->getPercentage() - 15);
	process->setTag(MonsterIsDecreasingHp);
	bar->runAction(process);
	if (monster->getUserData() != nullptr && ((MonsterInfo *)monster->getUserData())->hp <= 0) {
		Factory::getInstance()->removeMonster(monster);
		return true;
	}
	else return false;
}
