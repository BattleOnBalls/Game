#include "EnterScene.h"
#include "Scene/SceneManager/SceneManager.h"
#include "Client/web.h"

Scene * EnterScene::createScene()
{
	auto scene = Scene::create();
	auto layer = EnterScene::create();
	scene->addChild(layer);
	return scene;
}

bool EnterScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto title = Sprite::create("enterScene/enter_background1.png");

	Size size = Director::getInstance()->getVisibleSize();
	title->setPosition(Vec2(size.width / 2, size.height / 2));
	Size mywinsize = Director::getInstance()->getWinSize();
	float winw = mywinsize.width; //获取屏幕宽度
	float winh = mywinsize.height;//获取屏幕高度
	float spx = title->getTextureRect().getMaxX();
	float spy = title->getTextureRect().getMaxY();
	title->setScaleX(winw / spx); //设置精灵宽度缩放比例
	title->setScaleY(winh / spy);

	this->addChild(title);

	this->scheduleOnce(schedule_selector(EnterScene::enterMenuScene), 1);
	//想要切换成其他场景请将更改schedule_selector的参数
	return true;
}

EnterScene::EnterScene()
{

}
EnterScene::~EnterScene()
{

}

void EnterScene::enterMenuScene(float dt)
{
	SceneManager::getInstance()->changeScene(SceneManager::en_MenuScene);
}