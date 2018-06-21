#include "DataLayer.h"
#include "Header/Macros.h"
#include "Scene/SceneManager/SceneManager.h"
#include "Entity/Player.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"


DataLayer::DataLayer()
{

}

DataLayer::~DataLayer()
{
	//rank_list.clear();  //释放rank_list里所有Label指针指向的内存
	//移除所有自定义监听器
	//_eventDispatcher->removeCustomEventListeners("RankChange");
	_eventDispatcher->removeCustomEventListeners("ScoreChange");
	_eventDispatcher->removeCustomEventListeners("UpdateTime");
}

//初始化datalayer层，包括了里面的精灵、菜单和文字等内容
bool DataLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	//左上角的计分(也就是显示你多重)框框
	auto scoreBackground = Sprite::create("GameScene/score_background.png");
	scoreBackground->setPosition(73, 433);
	this->addChild(scoreBackground, 0);
	//然后把分数在框框上展示
	score_label = Label::createWithTTF("", "fonts/arial.ttf", 14);
	score_label->setAnchorPoint(Vec2(0, 0));
	score_label->setPosition(Vec2(23, 427));
	this->addChild(score_label, 1);

	/*
	//排行榜(排行榜三个字)
	auto rankItem = Sprite::create("gameScene/rank.png");
	rankItem->setPosition(712, 433);
	this->addChild(rankItem, 0);


	//排行榜每个玩家的框框用九宫图(Scale9Sprite)实现
	rank1 = Scale9Sprite::create("gameScene/game_scale9.png");
	rank1->setContentSize(Size(160, 100));
	rank1->setAnchorPoint(Vec2(0.5, 1));
	rank1->setPosition(712, 417);
	this->addChild(rank1, 0);

	rank2 = Scale9Sprite::create("gameScene/game_scale9.png");
	rank2->setContentSize(Size(160, 20));
	rank2->setAnchorPoint(Vec2(0.5, 1));
	rank2->setPosition(712, 314);
	this->addChild(rank2, 0);

	rank3 = Scale9Sprite::create("gameScene/game_scale9.png");
	rank3->setContentSize(Size(160, 20));
	rank3->setAnchorPoint(Vec2(0.5, 1));
	rank3->setPosition(712, 291);
	this->addChild(rank3, 0);

	for (int i = 0; i < RANK_MAX_ITEM_NUM; i++)
	{
	auto item = Label::createWithTTF("", "fonts/STSONG.TTF", 14);
	item->setColor(Color3B(220, 220, 220));
	item->setAnchorPoint(Vec2(0, 1));
	item->setPosition(653, 415 - i * RANK_ITEM_HEIGHT);
	this->addChild(item, 1);
	rank_list.pushBack(item);
	}
	*/

	//屏幕上方中央的时间标签
	time_label = Label::createWithTTF("", "fonts/arial.ttf", 18);
	time_label->setPosition(Vec2(400, 435));
	this->addChild(time_label, 1);


	_eventDispatcher->addCustomEventListener("ScoreChange", CC_CALLBACK_1(DataLayer::updateScore, this));
	//_eventDispatcher->addCustomEventListener("RankChange", CC_CALLBACK_1(DataLayer::updateRank, this));
	_eventDispatcher->addCustomEventListener("UpdateTime", CC_CALLBACK_1(DataLayer::updateTime, this));

	return true;
}

//更新玩家分数
void DataLayer::updateScore(EventCustom *event)
{
	int *score = (int*)(event->getUserData());                          //收到事件分发时设置的UserData(此处即分数 int*)
	score_label->setString(StringUtils::format("Score: %d", *score));
}

//更新时间(不如说是同步时间)
void DataLayer::updateTime(EventCustom *event)
{
	char *msg = (char*)event->getUserData();                            //收到事件分发时设置的UserData
	rapidjson::Document doc;
	doc.Parse<0>(msg);                                                  //解析msg(0表示解析标识，是默认值)

																		//如果解析错误，抛出异常
	if (doc.HasParseError())
	{
		log("GetParseError %d\n", doc.GetParseError());
		return;
	}
	//判断doc是否是有效的JSON对象
	if (doc.IsObject())
	{
		double time_count = doc["TimeCount"].GetDouble();               //获取doc对象里TimeCount对应的值(double)
		int left_time = GAME_TOTAL_TIME - (int)time_count;
		int minute = left_time / 60;
		int second = left_time % 60;
		//这里或许⋯⋯可以在秒数为一位数时加个0？
		time_label->setString(StringUtils::format("%d:%d", minute, second));

		//如果时间到了，那么返回MenuScene
		if (left_time <= 0)
		{
			SceneManager::getInstance()->changeScene(SceneManager::en_MenuScene);
		}
	}
}
/*
//更新排行榜
void DataLayer::updateRank(EventCustom *event)
{
//收到事件分发时设置的UserData(此处即所有玩家)
auto vec = static_cast<Vector<Player*>*>(event->getUserData());

int count = 0;
for (auto player : *vec)
{
auto item = rank_list.at(count);
count++;
std::string text = StringUtils::format("%d  %s", count, player->getPlayerName().c_str());
item->setString(text);
if (count >= RANK_MAX_ITEM_NUM)
{
break;
}
}

int height = 20 * count + 4;
rank1->setContentSize(Size(160, height));
Vec2 position = rank1->getPosition();
rank2->setPosition(position.x, position.y - height - RANK_ITEM_MARGIN);
rank3->setPosition(position.x, position.y - height - RANK_ITEM_HEIGHT - RANK_ITEM_MARGIN * 2);
}
*/

void DataLayer::onExit()
{
	Layer::onExit();
}