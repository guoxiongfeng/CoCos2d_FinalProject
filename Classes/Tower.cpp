#include "Tower.h"
#include "Monster.h"
#define HIT 50
#define BULLET -2
#define TOWER 2
#define BULLET_LIMIT 2
USING_NS_CC;
TowerFactory* TowerFactory::factory = NULL;


TowerFactory::TowerFactory() {
	attackRadius = 300.0f;
}

TowerFactory* TowerFactory::getInstance() {
	if (factory == NULL) {
		factory = new TowerFactory();
	}
	return factory;
}

Sprite * TowerFactory::createTower()
{
	Sprite* tow = Sprite::create("Tower.png");
	tow->setAnchorPoint(Vec2(0.5, 0.5));
	tow->setScaleX(0.3);
	tow->setScaleY(0.3);
	tow->setName("tower");
	tower.pushBack(tow);
	return tow;
}

void TowerFactory::Attack()
{
	auto monsters = Factory::getInstance()->monster;
	for (auto it = tower.begin(); it != tower.end(); ++it) {
		int temp = (*it)->getChildrenCount();
		if ((*it)->getChildren().size() >= BULLET_LIMIT) continue;
		double minDisSqr = 1000000; Sprite* nearest = nullptr;
		for (auto mons = monsters.begin(); mons != monsters.end(); ++mons) {
			if ((*it)->getPosition().getDistanceSq((*mons)->getPosition()) < minDisSqr) {
				minDisSqr = (*it)->getPosition().getDistanceSq((*mons)->getPosition());
				nearest = (*mons);
			}
		}
		if ( nearest != nullptr && (*it)->getPosition().getDistanceSq(nearest->getPosition()) < TowerFactory::attackRadius * TowerFactory::attackRadius) {
			//小于攻击半径， 打。
			auto bullet = createBullet((*it)->getPosition(), nearest->getPosition() /*+ nearest->getContentSize() / 2*/, 400);
			bullet->setPosition((*it)->getContentSize() / 2);
			bullet->setTag(BULLET);
			//Director::getInstance()->getRunningScene()->addChild(bullet);
			(*it)->addChild(bullet);
		}
	}
}


Sprite* TowerFactory::createBullet(Vec2 currentPos, Vec2 dstPos, double speed = 400.0) {
	Sprite* bullet = Sprite::create("bullet.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 0,73, 74)));

	//bullet->setScaleX(0.25);
	//bullet->setScaleY(0.25);
	//bullet->setPosition(currentPos);
	dstPos -= currentPos;
	bullet->setPosition(Vec2(0.5, 0.5));
	bullet->setAnchorPoint(Vec2(0.5, 0.5));
	bullet->setName("bullet");

	PhysicsBody* rigidbody = PhysicsBody::createBox(bullet->getContentSize());
	rigidbody->setGravityEnable(false);
	rigidbody->setTag(BULLET);
	rigidbody->setCategoryBitmask(0x0000FFFF);
	rigidbody->setCollisionBitmask(0x0000FFFF);
	rigidbody->setContactTestBitmask(0x0000FFFF);
	bullet->addComponent(rigidbody);




	auto dist = Vec2::ZERO.getDistance(dstPos);
	// t = s / v; 1.02弥补刚好够不到的不足。
	auto hit = Sequence::create(MoveBy::create(dist / speed, 3.4 * (dstPos - bullet->getPosition())), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, bullet)),nullptr);
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
