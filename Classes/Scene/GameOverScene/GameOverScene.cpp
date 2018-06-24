#include "GameOverScene.h"
#include "Scene/SceneManager/SceneManager.h"
#include "Macros.h"

Scene* GameOverScene::createScene()
{
	return GameOverScene::create();
}

bool GameOverScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//设置背景
	auto background = Sprite::create("GameOverScene/background.png");
	background->setScaleX(visibleSize.width / background->getContentSize().width);
	background->setScaleY(visibleSize.height / background->getContentSize().height);
	background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(background, 0);

	//退出游戏按钮
	auto closeItem = MenuItemImage::create("GameOverScene/over.png","GameOverScene/over.png",
		CC_CALLBACK_1(GameOverScene::gameoverCloseCallback, this));
	closeItem->setPosition(Vec2(origin.x + visibleSize.width / 4,
		origin.y + visibleSize.height / 4));

	//返回大厅按钮
	auto returnItem = MenuItemImage::create("GameOverScene/return.png", "GameOverScene/return_se.png",
		CC_CALLBACK_1(GameOverScene::gameoverReturnCallback, this));
	returnItem->setPosition(Vec2(origin.x + 3 * visibleSize.width / 4,
		origin.y + visibleSize.height / 4));

	auto menu = Menu::create(closeItem, returnItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	//分数显示
	auto titlebox = Sprite::create("GameOverScene/score_box.png");
	titlebox->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + 3 * visibleSize.height / 4));
	this->addChild(titlebox, 1);
	auto title= Label::createWithTTF("Your Score", "fonts/Marker Felt.ttf", 26);
	title->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + 3 * visibleSize.height / 4));
	this->addChild(title, 2);
	auto scorelabel = Label::createWithTTF("", "fonts/Marker Felt.ttf", 60);
	scorelabel->setString(StringUtils::format("%d", SCORE));
	scorelabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	scorelabel->setColor(Color3B::BLACK);
	this->addChild(scorelabel, 2);

	return true;
}


void GameOverScene::gameoverCloseCallback(Ref *pSender)
{
	//Close the cocos2d-x game scene and quit the application
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif

	/*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

	//EventCustom customEndEvent("game_scene_close_event");
	//_eventDispatcher->dispatchEvent(&customEndEvent);
}

void GameOverScene::gameoverReturnCallback(Ref *pSender)
{
	SceneManager::getInstance()->changeScene(SceneManager::en_MenuScene);
}