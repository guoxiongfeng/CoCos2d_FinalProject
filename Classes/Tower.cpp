#include "Tower.h"
#include "Monster.h"
#define HIT 50
#define BULLET -2
USING_NS_CC;
TowerFactory* TowerFactory::factory = NULL;


TowerFactory::TowerFactory() {
	attackRadius = 500.0f;
}

TowerFactory* TowerFactory::getInstance() {
	if (factory == NULL) {
		factory = new TowerFactory();
	}
	return factory;
}

Sprite * TowerFactory::createTower()
{
	Sprite* tow = Sprite::create("tempTower.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 64, 64)));
	//tow->setScaleX(0.05);
	//tow->setScaleY(0.05);
	tower.pushBack(tow);
	return tow;
}

void TowerFactory::Attack()
{
	auto monsters = Factory::getInstance()->monster;
	for (auto it = tower.begin(); it != tower.end(); ++it) {
		double minDisSqr = 1000000; Sprite* nearest = nullptr;
		for (auto mons = monsters.begin(); mons != monsters.end(); ++mons) {
			if ((*it)->getPosition().getDistanceSq((*mons)->getPosition()) < minDisSqr) {
				minDisSqr = (*it)->getPosition().getDistanceSq((*mons)->getPosition());
				nearest = (*mons);
			}
		}
		if ( nearest != nullptr && (*it)->getPosition().getDistanceSq(nearest->getPosition()) < TowerFactory::attackRadius * TowerFactory::attackRadius) {
			//Ð¡ÓÚ¹¥»÷°ë¾¶£¬ ´ò¡£
			auto bullet = createBullet((*it)->getPosition(), nearest->getPosition(), 400);
			Director::getInstance()->getRunningScene()->addChild(bullet);
		}
	}
}


Sprite* TowerFactory::createBullet(Vec2 currentPos, Vec2 dstPos, double speed = 400.0) {
	Sprite* bullet = Sprite::create("bullet.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 0,73, 74)));
	bullet->setScaleX(0.25);
	bullet->setScaleY(0.25);
	bullet->setPosition(currentPos);
	bullet->setName("bullet");

	PhysicsBody* rigidbody = PhysicsBody::createBox(bullet->getContentSize());
	rigidbody->setGravityEnable(false);
	rigidbody->setTag(BULLET);
	rigidbody->setCategoryBitmask(0xFFFFFFFF);
	rigidbody->setCollisionBitmask(0xFFFFFFFF);
	rigidbody->setContactTestBitmask(0xFFFFFFFF);
	bullet->addComponent(rigidbody);




	auto dist = currentPos.getDistance(dstPos);
	// t = s / v;
	auto hit = Sequence::create(MoveTo::create(dist / speed, dstPos), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, bullet)),nullptr);
	hit->setTag(HIT);
	bullet->runAction(hit);
	return bullet;
}

void TowerFactory::removeTower(Sprite * tow)
{
	tow->removeFromParent();
	tower.eraseObject(tow);
}

void TowerFactory::removeAllTower()
{
	tower.clear();
}
