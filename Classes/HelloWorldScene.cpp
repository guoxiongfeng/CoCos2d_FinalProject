#include "HelloWorldScene.h"
#include "Monster.h"
#include "Tower.h"
#include "SimpleAudioEngine.h"
#define PLAYER 1

#define JUMP 99
#define DEAD 18
#define ATTACK 36
#define MOVE 10
#define CHANGING_POS 15
#define BloodBar -30000
#define PlayerIsDecreasingHp -30002
#define MONSTER_DAMAGE 5
#define REVIVETIME 20
/*测试用， 正式需修改  测试时CD全为3即可*/
#define COLD_TIME_0 5
#define COLD_TIME_1 10
#define COLD_TIME_2 15
#define COLD_TIME_3 3


#define LEVEL_3_SKILL_TIME 5
#define LEVEL_4_SKILL_TIME 7
#define LIMITTOWERNUM 5 + HelloWorld::currentScore / 500
#define PLAYER_LEVEL 1 + (int)floor(sqrt(HelloWorld::currentScore / 300))
#pragma execution_character_set("utf-8")
USING_NS_CC;
using namespace CocosDenshion;
int HelloWorld::currentScore = 0;
bool HelloWorld::Level_5_Skill_On = false;
Scene* HelloWorld::createScene()
{
    auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setAutoStep(false);
	scene->getPhysicsWorld()->setGravity(Vec2::ZERO);

	//开启DEBUG模式
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
	
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	setMap();
	AnimationInit();
	InitialScene();
	SetProgressBar();
	setEvent();
	addTouchListener();
	addContactListener();
	//缓慢恢复player以及home的血量
	schedule(schedule_selector(HelloWorld::SlowHeal), 8.0f, CC_REPEAT_FOREVER, 0);
	schedule(schedule_selector(HelloWorld::UpdateColdTime), 1.0f, CC_REPEAT_FOREVER, 0);
	schedule(schedule_selector(HelloWorld::TowerAttack), 0.02f, CC_REPEAT_FOREVER, 0);
	schedule(schedule_selector(HelloWorld::update), 0.01f, kRepeatForever, 0.1f); //physic world
	schedule(schedule_selector(HelloWorld::hitByMonster), 0.02f, CC_REPEAT_FOREVER, 0);
	schedule(schedule_selector(HelloWorld::detactKeyboardEvent), 0.02f, CC_REPEAT_FOREVER,0);
	//Monster要越来越多才好， 符合游戏规律:) 
	schedule(schedule_selector(HelloWorld::createMonster), 0.4f, CC_REPEAT_FOREVER, 0);
	schedule(schedule_selector(HelloWorld::moveMonster), 0.02f, CC_REPEAT_FOREVER, 0);
	


	SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
	return true;
}
//Undebuged Module.
void HelloWorld::SlowHeal(float dt) {
	auto homeHeal = ProgressFromTo::create(0.5, pT->getPercentage(), pT->getPercentage() + 1);
	pT->runAction(homeHeal);
	auto temp = (ProgressTimer *)player->getChildByTag(BloodBar);
	auto playerHeal = ProgressFromTo::create(0.5, temp->getPercentage(), temp->getPercentage() + 1);
	temp->runAction(playerHeal);

	
}

void HelloWorld::addContactListener() {
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onConcactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

//暂时应该只有子弹和monster的碰撞。
//bullet相撞
bool HelloWorld::onConcactBegin(PhysicsContact & contact) {
	Sprite *bullet = nullptr, *monster = nullptr;
	if (contact.getShapeA()->getBody()->getNode() == nullptr || contact.getShapeB()->getBody()->getNode() == nullptr)
		return false;
	if (contact.getShapeA()->getBody()->getNode()->getName() ==  contact.getShapeB()->getBody()->getNode()->getName())
		return false;
	if (contact.getShapeA()->getBody()->getNode()->getName() == "bullet") {
		bullet = (Sprite *)contact.getShapeA()->getBody()->getNode();
		monster = (Sprite *)contact.getShapeB()->getBody()->getNode();
	}
	else {
		monster = (Sprite *)contact.getShapeA()->getBody()->getNode();
		bullet = (Sprite *)contact.getShapeB()->getBody()->getNode();
	}

	bullet->removeFromParent();//此处应有动画。
	if (Factory::getInstance()->DecreaseHP(monster, true)) {
		//这样可能会多次增加分数
		//currentScore += 10;
		setText();
	}
	return true; //可以试试false
}

void HelloWorld::TowerAttack(float dt) {
	TowerFactory::getInstance()->Attack();
}
void HelloWorld::update(float dt) {
	this->getScene()->getPhysicsWorld()->step(1 / 100.0f);
}

void HelloWorld::MoveCallback(Ref* pSender)
{
	
	//if (isAnimateExcuting) return;
	/*
	0-A 1-S 2-D 3-W
	4-X 5-Y
	*/
	int k = ((MenuItemLabel *)pSender)->getTag();
	switch (k) {
	case 0: Move(Vec2(-40, 0)); player->setFlippedX(true); return;
	case 1: Move(Vec2(0, -40)); return;
	case 2: Move(Vec2(40, 0)); player->setFlippedX(false); return;
	case 3: Move(Vec2(0, 40)); return;
		//设置另外的动画， 而不是dead动画。
	case 4: Jump(); return;
	case 5: Attack(); return;
	default: ;
	}
	//((MenuItemLabel *)pSender)->getLabel()->get;
	
}

//Jump Tag: JUMP
void HelloWorld::Jump() {
	if (player->getActionByTag(JUMP) != NULL) return;
	Vec2 by(40, 0);
	if (player->isFlippedX()) by.x = -by.x;
	if (pressTable[EventKeyboard::KeyCode::KEY_S]) by.y += -40;
	if (pressTable[EventKeyboard::KeyCode::KEY_W]) by.y += 40;
	if (!isValid(player->getPosition() , by)) return;
	auto Jump = JumpBy::create(0.5f, by, 30.0f, 1);
	
	Jump->setTag(JUMP);
	player->runAction(Jump);
	
}

bool HelloWorld::CheckAnimation() {
	auto t = player->getActionByTag(DEAD);
	if (t != NULL) return false;
	return true;
}
Sequence* HelloWorld::addStatic(Animate * t) {
	auto idleAnimation = Animation::createWithSpriteFrames(idle, 0.1f);
	auto idleAnimate = Animate::create(idleAnimation);
	auto together = Sequence::createWithTwoActions(t, idleAnimate);
	return together;
}
void HelloWorld::Dead() {
	if (!CheckAnimation()) return;
	auto isAttacking = player->getActionByTag(ATTACK);
	if (isAttacking != NULL) {
		player->getActionManager()->removeAction(isAttacking);
	}
	auto deadAnimation = Animation::createWithSpriteFrames(dead, 0.08f);
	auto deadAnimate = Animate::create(deadAnimation);

	auto together = Sequence::create(deadAnimate, CallFunc::create(CC_CALLBACK_0(HelloWorld::DisablePlayer, this)),
	NULL);
	together->setTag(DEAD);
	
	player->runAction(together);

	schedule(schedule_selector(HelloWorld::Reviving), 1.0f, CC_REPEAT_FOREVER, 0);
	reviveTime->setVisible(true);
	HelloWorld::Level_5_Skill_On = false;
}

void HelloWorld::Reviving(float dt) {
	static int restTime = REVIVETIME; //
	if (restTime > 0) {
		restTime--;
		reviveTime->setString("Reviving CD: " + std::to_string(restTime) + "s");
	}
	else {
		restTime = REVIVETIME;
		//revive player
		Revive(visibleSize / 2);
		reviveTime->setVisible(false);
		pT->setPercentage(100);
		unschedule(schedule_selector(HelloWorld::Reviving));
	}
}

//此处可加特效
void HelloWorld::Revive(Vec2 pos) {
	player->setPosition(pos);
	player->setVisible(true);
	ProgressTimer* bloodBar = (ProgressTimer*) player->getChildByTag(BloodBar);
	bloodBar->setPercentage(100);
}

void HelloWorld::DisablePlayer() {
	player->setVisible(false);
	player->setPosition(Vec2(visibleSize.width / 2, visibleSize.height + 200));//设置死亡角色在界外
}

void HelloWorld::Attack() {
	if (!CheckAnimation() || player->getActionByTag(ATTACK) != NULL) return;
	auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.03f);
	auto attackAnimate = Animate::create(attackAnimation);
	auto together = addStatic(attackAnimate);
	together->setTag(ATTACK);
	player->runAction(together);

}
void HelloWorld::setText() {
	auto database = UserDefault::getInstance();
	int k = database->getIntegerForKey("Number");
	if (k < currentScore)
		database->setIntegerForKey("Number", currentScore);
	time->setString("Score: " + std::to_string(currentScore));
	//if (currentScore % 1000 == 0)
		UpdateValidTower();
		UpdatePlayerLevel();
}

void HelloWorld::UpdatePlayerLevel() {
	PlayerLevel->setString("Player Level: " + std::to_string(PLAYER_LEVEL));
}

void HelloWorld::Move(Vec2 direction) {
	if (!CheckAnimation()) return;
	if (player->getActionByTag(ATTACK)!= NULL) return;
	auto runAnimation = Animation::createWithSpriteFrames(run, 0.05f);
	if (player->getActionByTag(MOVE) == nullptr) {
		auto runAnimate = RepeatForever::create(Animate::create(runAnimation));
		player->runAction(runAnimate);
		runAnimate->setTag(MOVE);
	}

	Vec2 newPosition = player->getPosition() + direction;
	if (isValid(player->getPosition(), direction)) {
		auto action = MoveTo::create(0.4f, newPosition);
		action->setTag(CHANGING_POS);
		player->runAction(action);
	}
	
	
	//isAnimateExcuting = true;
}

bool HelloWorld::isValid(Vec2 now, Vec2 by) {
	Vec2 center(visibleSize.width / 2, visibleSize.height / 2);
	Vec2 t = now + by;
	double width = player->getBoundingBox().getMaxX() - player->getBoundingBox().getMinX();
	double height = player->getBoundingBox().getMaxY() - player->getBoundingBox().getMinY();
	return ((t.x > width && t.x < visibleSize.width - width && t.y  > 0 && t.y < visibleSize.height) || t.distance(center) < now.distance(center));
}

std::string HelloWorld::setLabelText(int k) {
	switch (k)  {
	case 0: return "A";
	case 1: return "S";
	case 2: return "D";
	case 3: return "W";
	case 4: return "X";
	case 5: return "Y";
	default: return "?";
	}
}

void HelloWorld::createMonster(float dt) {

	Factory::getInstance()->moveMonster(player->getPosition(), 0.5f);
	if (RandomHelper::random_int(0, 20) < 18 - currentScore / 300) return; //随着score增加出现monster更快。
	auto monster = Factory::getInstance()->createMonster();
	monster->setPosition(visibleSize.width + RandomHelper::random_real(0.0f, 10.0f), RandomHelper::random_real(80.0f, visibleSize.height - 80));
	this->addChild(monster, 3);
}

void HelloWorld::moveMonster(float dt) {
	Factory::getInstance()->moveMonster(player->getPosition(), 0.5f);//
}

void HelloWorld::InitialScene() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/bgm.mp3");
	audio->preloadBackgroundMusic("music/fail.mp3");
	//set Parameter
	attackOfPlayer = 10;
	resumeFromAttack = 2;
	//set score
	time = Label::create("Score: 0", "arial", 36);
	time->setPosition(visibleSize.width / 2, visibleSize.height - 60);
	
	this->addChild(time, 120);

	// show valid tower number.
	validTowerNum = Label::create("Valid Tower: " + std::to_string(LIMITTOWERNUM), "arial", 20);
	validTowerNum->setPosition(visibleSize.width / 5 * 4, visibleSize.height - 60);
	this->addChild(validTowerNum, 120);

	PlayerLevel = Label::create("Player Level: " + std::to_string(PLAYER_LEVEL), "arial", 20);
	PlayerLevel->setPosition(visibleSize.width / 5 * 4, visibleSize.height - 100);
	this->addChild(PlayerLevel, 120);

	setText();

	// 设置复活时间显示（不处于死亡状态不显示
	reviveTime = Label::create("Reviving CD: 0s", "arial", 20);
	reviveTime->setPosition(visibleSize.width / 2, visibleSize.height - 90);
	reviveTime->setVisible(false);
	this->addChild(reviveTime, 120);

	//set home
	home = Sprite::create();
	home->setContentSize(Size(10, visibleSize.height));
	home->setPosition(Vec2(-10, visibleSize.height / 2));

	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	this->addChild(home, 3);

	//设置技能列表
	skill_icon[0] = Sprite::create("wind.png");
	skill_icon[1] = Sprite::create("attackinc.png");
	skill_icon[2] = Sprite::create("resume.png");
	skill_icon[3] = Sprite::create("attract.png");
	skill_icon[4] = Sprite::create("unknown.png");

	for (int i = 0; i < 4; ++i) {
		skill_icon[i]->setPosition(Vec2(250 + 62 * i, 40));
		addChild(skill_icon[i], 120);
		skill_icon[i]->setVisible(false);
	}

	
	//设置玩家血条 需要更好的血条素材
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));
	sp->setColor(Color3B::BLUE); //区别于其他的血条
	auto temppT = ProgressTimer::create(sp);
	temppT->setScaleX(16);
	temppT->setScaleY(0.5);
	temppT->setAnchorPoint(Vec2(0.0, 0.0));
	temppT->setType(ProgressTimerType::BAR);
	temppT->setBarChangeRate(Point(1, 0));
	temppT->setMidpoint(Point(0, 1));
	temppT->setPercentage(100);
	temppT->setPosition((Vec2((player->getContentSize().width - 60) / 2, player->getContentSize().height / 1.1)));
	temppT->setTag(BloodBar);
	player->addChild(temppT, 1);


	//设置玩家的刚体
	addChild(player, 10);



	Node * Labels[6];
	MenuItemLabel * MenuLabels[6];
	Vec2 LabelPosition[6] = { Vec2(20 ,40), Vec2(60 ,40), Vec2(100 ,40), Vec2(60 ,80), 
		Vec2(visibleSize.width - 30 ,80), Vec2(visibleSize.width -  70,40) };
	
	//少加一个 隐藏Y

	for (int i = 0; i < 6; ++i) {
		Labels[i] = Label::create(setLabelText(i), "arial", 36.0f);
		
		MenuLabels[i] = MenuItemLabel::create(Labels[i], CC_CALLBACK_1(HelloWorld::MoveCallback, this));
		MenuLabels[i]->setPosition(LabelPosition[i] + origin);
		MenuLabels[i]->setTag(i);
	}
	Menu* menu = Menu::create(MenuLabels[0], MenuLabels[1], MenuLabels[2], MenuLabels[3], MenuLabels[4], MenuLabels[5], NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 4);
}


//设置地图。 地图需要更换。
void HelloWorld::setMap() {
	Sprite * tmx = Sprite::create("map.png");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	this->addChild(tmx, 0);
}



//动画资源找到了切割就放这里
void HelloWorld::AnimationInit() {



	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	// 攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}
	auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.1f);
	auto attackAnimate = Animate::create(attackAnimation);


	//死亡动画
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	//运动动画
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	run.reserve(8);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	/*
	auto frame4 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 68, 101)));
	run.pushBack(frame4);
	*/
}

void HelloWorld::SetProgressBar() {
	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);
}


bool HelloWorld::contactWithTower() {
	auto tower = TowerFactory::getInstance()->tower;
	bool flag = false;
	for (auto i = tower.begin(); i != tower.end(); ++i) {
		if ((*i)->getBoundingBox().containsPoint(player->getPosition()))
			return true;
	}
	return false;
}

void HelloWorld::hitByMonster(float dt) {
	//顺便检测player 与 tower 的碰撞情况。
	if (contactWithTower() && player->getActionByTag(MOVE) != nullptr) {
		player->stopActionByTag(MOVE);
	}
	auto fac = Factory::getInstance();
	//也可用home boundedBox检测。
	auto collider = fac->collider(home->getBoundingBox());
	if (collider != nullptr) {
		fac->removeMonster(collider);
		auto progress = ProgressFromTo::create(0.5f, pT->getPercentage(), pT->getPercentage() - 10);
		pT->runAction(progress);
	}
	if (pT->getPercentage() <= 0) GameOver();


	if (player->getActionByTag(ATTACK) != NULL) {
		Rect playerRect = player->getBoundingBox();
		Rect AttackRect = Rect(playerRect.getMinX() - MOVE, playerRect.getMinY(),
			playerRect.getMaxX() - playerRect.getMinX() + 20, playerRect.getMaxY() - playerRect.getMinY());
		Sprite *collision = Factory::getInstance()->collider(AttackRect);

		//击中， 怪掉血
		if (collision != NULL) {
			if (Factory::getInstance()->DecreaseHP(collision, false)) {
				auto temppT = (ProgressTimer *)player->getChildByTag(BloodBar);
				auto process = ProgressFromTo::create(0.5f, temppT->getPercentage(), temppT->getPercentage() + resumeFromAttack + attackOfPlayer / 5); //玩家回复。
				temppT->runAction(process);
				/*
				*/
				currentScore += 100;
				setText();
			}
			//else do nothing.
		}
	}
		//被击中， player掉血

		Sprite *collision = fac->collider(player->getBoundingBox());
		if (collision != NULL) {
			//fac->removeMonster(collision);
			playerDecreaseBlood(MONSTER_DAMAGE);//玩家掉的血量。
		}
	}

void HelloWorld::playerDecreaseBlood(int amount) {
	auto temppT = (ProgressTimer *)player->getChildByTag(BloodBar);
	if (temppT->getActionByTag(PlayerIsDecreasingHp) != nullptr) return; //正在减血不执行下面的操作
	int currentHp = temppT->getPercentage();
	auto process = ProgressFromTo::create(0.8f, currentHp, currentHp - amount);
	process->setTag(PlayerIsDecreasingHp);
	temppT->runAction(process);
	if (currentHp - amount <= 0) 
		Dead();
}


void HelloWorld::GameOver() {
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	SimpleAudioEngine::getInstance()->playBackgroundMusic("music/fail.mp3", false);
	unschedule(schedule_selector(HelloWorld::hitByMonster));
	unschedule(schedule_selector(HelloWorld::detactKeyboardEvent));
	unschedule(schedule_selector(HelloWorld::createMonster));
	unschedule(schedule_selector(HelloWorld::moveMonster));
	unschedule(schedule_selector(HelloWorld::Reviving));
	unschedule(schedule_selector(HelloWorld::SlowHeal));
	this->getEventDispatcher()->removeAllEventListeners();

	Factory::getInstance()->removeAllMonster();
	TowerFactory::getInstance()->removeAllTower();
	HelloWorld::currentScore = 0;
	HelloWorld::Level_5_Skill_On = false;
	auto label1 = Label::createWithTTF("防守失败2333", "fonts/STXINWEI.TTF", 60);
	//label1->setColor(Color3B(0, 0, 0));
	label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label1, 100);

	auto label2 = Label::createWithTTF("重玩", "fonts/STXINWEI.TTF", 40);
	//label2->setColor(Color3B(0, 0, 0));
	auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(HelloWorld::replayCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
	this->addChild(replay, 100);

	auto label3 = Label::createWithTTF("退出", "fonts/STXINWEI.TTF", 40);
	//label3->setColor(Color3B(0, 0, 0));
	auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(HelloWorld::exitCallback, this));
	Menu* exit = Menu::create(exitBtn, NULL);
	exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
	this->addChild(exit, 100);
}

// 继续或重玩按钮响应函数
void HelloWorld::replayCallback(Ref * pSender) {
	Director::getInstance()->replaceScene(HelloWorld::createScene());
}

// 退出
void HelloWorld::exitCallback(Ref * pSender) {
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}




//下面的代码用于添加鼠标操作
// 添加触摸事件监听器
void HelloWorld::addTouchListener() {
	// Todo
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto touchListener = EventListenerTouchOneByOne::create();

	touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	//touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
	//touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);

	dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// 鼠标点击放置防御塔
bool HelloWorld::onTouchBegan(Touch *touch, Event *event) {
	Vec2 pos = touch->getLocation();
	auto &towers = TowerFactory::getInstance()->tower;
	for (auto tow = towers.begin(); tow != towers.end(); tow++) {
		if ((*tow)->getBoundingBox().containsPoint(pos)) {
			(*tow)->removeFromParent();
			towers.eraseObject(*tow);
			UpdateValidTower();
			return true;
		}
	}
	if (towers.size() >= LIMITTOWERNUM) return false;
	auto tow = TowerFactory::getInstance()->createTower();
	tow->setPosition(pos);
	this->addChild(tow, 20);
	UpdateValidTower();
	return true;
}

void HelloWorld::UpdateValidTower() {
	validTowerNum->setString("Valid Tower: " + std::to_string(LIMITTOWERNUM - TowerFactory::getInstance()->tower.size()));
}




//下面的代码用于添加键盘操作



void HelloWorld::detactKeyboardEvent(float dt) {
	if (pressTable[EventKeyboard::KeyCode::KEY_A] || pressTable[EventKeyboard::KeyCode::KEY_LEFT_ARROW]) {
		Move(Vec2(-100 * dt, 0)); player->setFlippedX(true);
	}
	if (pressTable[EventKeyboard::KeyCode::KEY_D] || pressTable[EventKeyboard::KeyCode::KEY_RIGHT_ARROW]) {
		Move(Vec2(100 * dt, 0)); player->setFlippedX(false);
	}
	if (pressTable[EventKeyboard::KeyCode::KEY_S] || pressTable[EventKeyboard::KeyCode::KEY_DOWN_ARROW]) {
		Move(Vec2(0, -100 * dt));
	}
	if (pressTable[EventKeyboard::KeyCode::KEY_W] || pressTable[EventKeyboard::KeyCode::KEY_UP_ARROW]) {
		Move(Vec2(0, 100 * dt)); 
	}
}


void HelloWorld::setEvent() {
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);
	dispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}
void HelloWorld::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	if (code == EventKeyboard::KeyCode::KEY_J) Attack();
	if (code == EventKeyboard::KeyCode::KEY_K) Jump();
	if (code == EventKeyboard::KeyCode::KEY_U) Level_2_Skill();
	
	if (code == EventKeyboard::KeyCode::KEY_I) Level_3_Skill();
	if (code == EventKeyboard::KeyCode::KEY_O) Level_4_Skill();
	if (code == EventKeyboard::KeyCode::KEY_P) Level_5_Skill();
	
	pressTable[code] = true;
}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	player->stopActionByTag(CHANGING_POS);
	player->stopActionByTag(MOVE);
	pressTable[code] = false;
}

void HelloWorld::Level_2_Skill() {
	if (!reviveTime->isVisible() && PLAYER_LEVEL >= 2 && skillColding[0] == 0) {
		auto monster = Factory::getInstance()->monster;
		
		skillColding[0] = COLD_TIME_0;
		//unschedule(schedule_selector(HelloWorld::moveMonster));
		for (auto mons = monster.begin(); mons != monster.end(); ++mons) {
			auto MoveBack = MoveBy::create(0.5, Vec2(100, 0));
			(*mons)->runAction(MoveBack);
		}
	}
	else {
		return;
	}

}

void HelloWorld::Level_3_Skill() {
	if (!reviveTime->isVisible() && PLAYER_LEVEL >= 3 && skillColding[1] == 0) {
		attackOfPlayer *= 2;
		skillColding[1] = COLD_TIME_1;
		ParticleFire * angry = ParticleFire::create();
		angry->setPosition(0, 0);
		angry->setAnchorPoint(Vec2(0.5, 0.5));
		angry->setStartSize(10);
		angry->setEndSize(0);
		angry->setDuration(LEVEL_3_SKILL_TIME);
		player->addChild(angry);
		schedule(schedule_selector(HelloWorld::Level3Effect), 1.0f, LEVEL_3_SKILL_TIME, 0);
	}
}

void HelloWorld::Level3Effect(float dt) {
	static int t = 5;
	t--;
	if (t == 0) attackOfPlayer /= 2;
}


void HelloWorld::Level_4_Skill() {
	if (!reviveTime->isVisible() && PLAYER_LEVEL >= 4 && skillColding[2] == 0) {
		skillColding[2] = COLD_TIME_2;
		resumeFromAttack += attackOfPlayer;
		ParticleGalaxy *buff = ParticleGalaxy::create();
		buff->setDuration(LEVEL_4_SKILL_TIME);
		buff->setEmitterMode(ParticleSystem::Mode::RADIUS);
		buff->setPosition(0, 0);
		buff->setAnchorPoint(Vec2(0.5, 0.5));
		buff->setStartRadius(20);
		buff->setEndRadius(0);
		player->addChild(buff);
		unschedule(schedule_selector(HelloWorld::SlowHeal));
		schedule(schedule_selector(HelloWorld::SlowHeal), 1.0f, CC_REPEAT_FOREVER, 0);
		schedule(schedule_selector(HelloWorld::Level4Effect), 1.0f, LEVEL_4_SKILL_TIME, 0);
	}
}

void HelloWorld::Level4Effect(float dt) {
	static int t = 5;
	static int incAmount = attackOfPlayer;
	t--;
	if (t == 0) {
		unschedule(schedule_selector(HelloWorld::SlowHeal));
		schedule(schedule_selector(HelloWorld::SlowHeal), 8.0f, CC_REPEAT_FOREVER, 0);
		resumeFromAttack -= incAmount;
	}
	
}

void HelloWorld::Level_5_Skill() {
	if (!reviveTime->isVisible() && PLAYER_LEVEL >= 5 && skillColding[3] == 0) {
		HelloWorld::Level_5_Skill_On = !HelloWorld::Level_5_Skill_On;
		skillColding[3] = COLD_TIME_3;
	}
}

void HelloWorld::UpdateColdTime(float dt) {
	for (int i = 0; i < 4; ++i) {
		if (skillColding[i] > 0 || PLAYER_LEVEL < 2 + i || reviveTime->isVisible()) {
			if (skillColding[i] > 0) 
				skillColding[i] --;
			skill_icon[i]->setVisible(false);
		}
		else {
			skill_icon[i]->setVisible(true);
		}
	}
}