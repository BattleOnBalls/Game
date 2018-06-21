#include "GameScene.h"
#include "GameLayer.h"
#include "DataLayer.h"
#include "Header/Macros.h"
#include "Header/MessageType.h"
#include "Client/web.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/reader.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
using namespace rapidjson;

enum GameZOrder
{
	GAME_BACKGROUND_Z,      //背景
	GAME_LAYER_Z,           //游戏
	GAME_DATA_Z,            //数据
	GAME_MENU_Z,            //菜单
};

Scene * GameScene::createScene()
{
	log("NOW CREATE GAME\n");

	auto scene = Scene::create();

	log("PASS?\n");

	auto layer = GameScene::create();

	scene->addChild(layer);

	return scene;
}

bool GameScene::init()
{
	if (!Layer::init())
	{
		log("INIT FALSE\n");
		return false;
	}

	log("NOW INIT GAME\n");

	//吐球
	auto spitItem = CheckBox::create("GameScene/spit_btn.png", "GameScene/spit_btn.png");
	spitItem->setPosition(Vec2(SPITE_POS_X, SPITE_POS_Y));
	spitItem->setZoomScale(-0.1f);
	spitItem->addEventListener(CC_CALLBACK_2(GameScene::menuSpitCallback, this));
	this->addChild(spitItem, GAME_MENU_Z);


	//分裂
	auto divideItem = CheckBox::create("GameScene/divide_btn.png", "GameScene/divide_btn.png");
	divideItem->setPosition(Vec2(DIVIDE_POS_X, DIVISDE_POS_Y));
	divideItem->setZoomScale(-0.1f);
	divideItem->addEventListener(CC_CALLBACK_2(GameScene::menuDivideCallback, this));
	this->addChild(divideItem, GAME_MENU_Z);


	//加入数据显示层
	auto dataLayer = DataLayer::create();
	this->addChild(dataLayer, GAME_DATA_Z);


	auto gameLayer = GameLayer::create();
	this->addChild(gameLayer, GAME_LAYER_Z);

	Document doc;
	doc.SetObject();
	Document::AllocatorType &allocator = doc.GetAllocator();
	doc.AddMember("MsgType", MessageType::eMsg_A_ASSUMPTION, allocator);
	StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> write(buffer);
	doc.Accept(write);
	const string msg = buffer.GetString();
	cl.sendMsg(msg);
	log("MASAKA!\n");

	return true;
}

//以下为回调函数
void GameScene::menuSpitCallback(Ref * pSender, CheckBox::EventType type)
{
	_eventDispatcher->dispatchCustomEvent("Spit");
}

void GameScene::menuDivideCallback(Ref * pSender, CheckBox::EventType type)
{
	_eventDispatcher->dispatchCustomEvent("Divide");
}