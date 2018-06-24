#include "GameScene.h"
#include "GameLayer.h"
#include "DataLayer.h"
#include "Macros.h"
#include "Scene/SceneManager/SceneManager.h"

enum GameZOrder
{
	GAME_BACKGROUND_Z,      //背景
	GAME_LAYER_Z,           //游戏
	GAME_DATA_Z,            //数据
	GAME_MENU_Z             //按钮
};

Scene* GameScene::createScene()
{

	return GameScene::create();
}

bool GameScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//吐孢子按钮
	auto spitItem = CheckBox::create("GameScene/spit_btn.png", "GameScene/spit_btn.png");
	spitItem->setPosition(Vec2(SPITE_POS_X, SPITE_POS_Y));
	spitItem->setZoomScale(-0.1f);
	spitItem->addEventListener(CC_CALLBACK_2(GameScene::menuSpitCallback, this));
	this->addChild(spitItem, GAME_MENU_Z);

	//分裂按钮
	auto divideItem = CheckBox::create("GameScene/divide_btn.png", "GameScene/divide_btn.png");
	divideItem->setPosition(Vec2(DIVIDE_POS_X, DIVISDE_POS_Y));
	divideItem->setZoomScale(-0.1f);
	divideItem->addEventListener(CC_CALLBACK_2(GameScene::menuDivideCallback, this));
	this->addChild(divideItem, GAME_MENU_Z);

	//返回大厅按钮
	auto returnItem = MenuItemImage::create("GameScene/returnMenu.png", "GameScene/returnMenu.png",
		CC_CALLBACK_1(GameScene::gameMenuCallback, this));
	returnItem->setPosition(Vec2(visibleSize.width - returnItem->getContentSize().width + origin.x,
		visibleSize.height - returnItem->getContentSize().height + origin.y));
	auto menu = Menu::create(returnItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, GAME_MENU_Z);

	//加入数据显示层
	auto dataLayer = DataLayer::create();
	this->addChild(dataLayer, GAME_DATA_Z);

	//加入游戏层
	auto gameLayer = GameLayer::create();
	this->addChild(gameLayer, GAME_LAYER_Z);

	return true;
}

//以下为回调函数
void GameScene::menuSpitCallback(Ref *pSender, CheckBox::EventType type)
{
	_eventDispatcher->dispatchCustomEvent("Spit");
}

void GameScene::menuDivideCallback(Ref *pSender, CheckBox::EventType type)
{
	_eventDispatcher->dispatchCustomEvent("Divide");
}

void GameScene::gameMenuCallback(Ref *pSender)
{
	SceneManager::getInstance()->changeScene(SceneManager::en_MenuScene);
}