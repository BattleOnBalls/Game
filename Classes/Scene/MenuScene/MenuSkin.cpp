#include "MenuSkin.h"

Scene* MenuSkin::createScene()
{
	auto scene = Scene::create();
	auto layer = MenuSkin::create();
	scene->addChild(layer);
	return scene;
}

bool MenuSkin::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//纯背景
	auto background = Sprite::create("MenuScene/skin_background.png");
	background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(background, 0);


	//以下都是选择的事情了
	auto backItem = MenuItemImage::create("menuScene/back.png", "menuScene/back_se.png",
		CC_CALLBACK_1(MenuSkin::menuBackCallback, this));
	backItem->setScale((visibleSize.height / 12) / backItem->getContentSize().height);
	backItem->setPosition(Vec2(visibleSize.width - backItem->getContentSize().width + origin.x,
		visibleSize.height - backItem->getContentSize().height + origin.y));

	auto usewidth = (visibleSize.width - 2 * backItem->getContentSize().width) / 3;
	auto useheight = (visibleSize.height - 2 * backItem->getContentSize().height) / 2;
	auto scalex = 3 * usewidth / 4;
	auto scaley = 3 * useheight / 4;
	auto scale = (scalex < scaley) ? scalex : scaley;

	auto skinItem1 = MenuItemImage::create("GameScene/skin_1.png", "GameScene/skin_1.png",
		CC_CALLBACK_1(MenuSkin::menuSkinCallback1, this));
	skinItem1->setScale(scale / skinItem1->getContentSize().width);
	skinItem1->setPosition(Vec2(origin.x + backItem->getContentSize().width + usewidth / 2,
		visibleSize.height + origin.y - backItem->getContentSize().height - useheight / 2));

	auto skinItem2 = MenuItemImage::create("GameScene/skin_2.png", "GameScene/skin_2.png",
		CC_CALLBACK_1(MenuSkin::menuSkinCallback2, this));
	skinItem2->setScale(scale / skinItem2->getContentSize().width);
	skinItem2->setPosition(Vec2(origin.x + backItem->getContentSize().width + 3 * usewidth / 2,
		visibleSize.height + origin.y - backItem->getContentSize().height - useheight / 2));

	auto skinItem3 = MenuItemImage::create("GameScene/skin_3.png", "GameScene/skin_3.png",
		CC_CALLBACK_1(MenuSkin::menuSkinCallback3, this));
	skinItem3->setScale(scale / skinItem3->getContentSize().width);
	skinItem3->setPosition(Vec2(origin.x + backItem->getContentSize().width + 5 * usewidth / 2,
		visibleSize.height + origin.y - backItem->getContentSize().height - useheight / 2));

	auto skinItem4 = MenuItemImage::create("GameScene/skin_4.png", "GameScene/skin_4.png",
		CC_CALLBACK_1(MenuSkin::menuSkinCallback4, this));
	skinItem4->setScale(scale / skinItem4->getContentSize().width);
	skinItem4->setPosition(Vec2(origin.x + backItem->getContentSize().width + usewidth / 2,
		origin.y + backItem->getContentSize().height + useheight / 2));

	auto skinItem5 = MenuItemImage::create("GameScene/skin_5.png", "GameScene/skin_5.png",
		CC_CALLBACK_1(MenuSkin::menuSkinCallback5, this));
	skinItem5->setScale(scale / skinItem5->getContentSize().width);
	skinItem5->setPosition(Vec2(origin.x + backItem->getContentSize().width + 3 * usewidth / 2,
		origin.y + backItem->getContentSize().height + useheight / 2));

	auto skinItem6 = MenuItemImage::create("GameScene/skin_6.png", "GameScene/skin_6.png",
		CC_CALLBACK_1(MenuSkin::menuSkinCallback6, this));
	skinItem6->setScale(scale / skinItem6->getContentSize().width);
	skinItem6->setPosition(Vec2(origin.x + backItem->getContentSize().width + 5 * usewidth / 2,
		origin.y + backItem->getContentSize().height + useheight / 2));

	auto menu = Menu::create(backItem, skinItem1, skinItem2, skinItem3, skinItem4, skinItem5, skinItem6, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	return true;
}

void MenuSkin::returnScene(int skinID)
{
	//要把skinID传给服务器orz
	//代码待补！！！！！！！！！！！！！！！！！！！！

	Director::getInstance()->popScene();
}

void MenuSkin::onExit()
{
	Layer::onExit();
}