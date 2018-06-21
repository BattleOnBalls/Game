#include "MenuScene.h"
#include "MenuSkin.h"
#include "Scene/MenuScene/MenuScene.h"
#include "Header/MessageType.h"
#include "Scene/SceneManager/SceneManager.h"
#include "Client/web.h"
#include <string>
#include <cstring>
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "ui/CocosGUI.h"
USING_NS_CC;


Scene* MenuScene::createScene()
{
	return MenuScene::create();
}

bool MenuScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//设置背景
	auto background = Sprite::create("MenuScene/background.png");
	background->setScaleX(visibleSize.width / background->getContentSize().width);
	background->setScaleY(visibleSize.height / background->getContentSize().height);
	background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(background, 0);

	//本轮昵称(不可点击)
	auto namesprite = Sprite::create("menuScene/name.png");
	namesprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, 17 * visibleSize.height / 24 + origin.y));
	this->addChild(namesprite, 1);
	/*
	//昵称框框(不可点击)
	auto namebox = Sprite::create("menuScene/nameLabel.png");
	Vec2 pos;
	pos.x = visibleSize.width / 2 + origin.x;
	pos.y = 7 * visibleSize.height / 12 + origin.y;
	float scale = (visibleSize.width / 2) / (namebox->getContentSize().width);
	namebox->setScale(scale);
	namebox->setPosition(pos);
	this->addChild(namebox, 1);

	//昵称条条上展示昵称
	srand(time(nullptr));
	int key = rand() % 6;
	Dictionary *contents = Dictionary::createWithContentsOfFile("fonts/name.xml");
	const char *title = ((String*)contents->objectForKey(StringUtils::format("%d", key)))->getCString();
	myname = title;
	//cli.setName(myname);  //将昵称保存下来
	auto namelabel = LabelTTF::create(title, "fonts/arial.TTF", 22);
	namelabel->setPosition(pos);
	this->addChild(name-label, 2);
	*/

	//输入框
	auto namebox = Sprite::create("menuScene/nameLabel.png");
	auto editbox = cocos2d::ui::EditBox::create(Size(namebox->getContentSize().width, namebox->getContentSize().height), cocos2d::ui::Scale9Sprite::create("menuScene/nameLabel.png"));
	Vec2 pos;
	pos.x = visibleSize.width / 2 + origin.x;
	pos.y = 7 * visibleSize.height / 12 + origin.y;
	float scale = (visibleSize.width / 2) / (namebox->getContentSize().width);
	editbox->setScale(scale);
	editbox->setPosition(pos);
	editbox->setMaxLength(10);//最多输入字符数目
	editbox->setText("Enter Name(English)");//初始化文字
	editbox->setFontColor(Color3B(250, 250, 250));//文字颜色；
	editbox->setFontSize(22);//文字大小
	this->addChild(editbox, 1);

	myName = editbox->getText();
	log("gameName = %s\n", myName.c_str());

	//皮肤按钮
	auto skinItem = MenuItemImage::create("menuScene/skin.png", "menuScene/skin_se.png",
		CC_CALLBACK_1(MenuScene::menuSkinCallback, this));
	skinItem->setScale((visibleSize.height / 6) / skinItem->getContentSize().height);
	skinItem->setPosition(Vec2(visibleSize.width - skinItem->getContentSize().width / 2 + origin.x,
		visibleSize.height - skinItem->getContentSize().height / 2 + origin.y));

	//设置开始游戏按钮
	auto setNameItem = MenuItemImage::create("menuScene/set0.png", "menuScene/set1.png");
	setNameItem->setCallback([&, editbox](Ref*obj) {
		//取得输入框里面的文字，输出到控制台
		myName = editbox->getText();
		log("myName = %s\n", myName.c_str());
		const char *p = myName.c_str();
		int sum = 0;
		for (int i = 0; i < myName.size(); i++)
		{
			if ((p[i] >= 'a' && p[i] <= 'z') || (p[i] >= 'A' && p[i] <= 'Z'))
			{
				sum++;
			}
		}
		if (sum == myName.size())
		{
			_nameSet = true;
			log("Here set true\n");
		}
		else
		{
			_nameSet = false;
		}
	});
	setNameItem->setPosition(Vec2(visibleSize.width + origin.x - setNameItem->getContentSize().width,
		7 * visibleSize.height / 12 + origin.y));
	setNameItem->setScale(scale);

	auto startItem = MenuItemImage::create("menuScene/Begin.png", "menuScene/Begin_se.png",
		CC_CALLBACK_1(MenuScene::menuStartCallBack, this));
	startItem->setPosition(Vec2(53 * visibleSize.width / 96 + origin.x,
		11 * visibleSize.height / 30 + origin.y));
	startItem->setScale(scale);

	auto menu = Menu::create(skinItem, setNameItem, startItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 2);

	_eventDispatcher->addCustomEventListener("GameStart", CC_CALLBACK_1(MenuScene::gameStartEvent, this));
	_nameSet = false;
	return true;
}


void MenuScene::menuSkinCallback(Ref *pSender)
{

	auto sc = MenuSkin::createScene();
	Director::getInstance()->pushScene(sc);
}


void MenuScene::menuStartCallBack(Ref *pSender)
{
	if (_nameSet)
	{
		log("gamestart!");
		rapidjson::Document doc;
		doc.SetObject();
		rapidjson::Document::AllocatorType & allocator = doc.GetAllocator();
		doc.AddMember("MsgType", MessageType::eMsg_START_GAME_SINGLE, allocator);
		doc.AddMember("GameName", rapidjson::Value(myName.c_str(), allocator), allocator);
		int _keywordID = cl.getSkin();
		doc.AddMember("KeywordID", _keywordID, allocator);

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer>write(buffer);
		doc.Accept(write);

		std::string msg = buffer.GetString();

		log("__________menuscene() Json:\n%s", msg.c_str());

		cl.sendMsg(buffer.GetString());
		//测试用
		//Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("GameStart", NULL);
	}
	else
	{
		log("please reset your name!");
	}

}

void MenuScene::gameStartEvent(EventCustom *event)
{
	log("THIS IS A MARK.\n");
	SceneManager::getInstance()->changeScene(SceneManager::en_GameScene);
}

void MenuScene::onExit()
{
	_eventDispatcher->removeCustomEventListeners("GameStart");
	Scene::onExit();
}