#include "DataLayer.h"
#include "Macros.h"
#include "Scene/SceneManager/SceneManager.h"

int SCORE = 0;

DataLayer::DataLayer()
{
	_time = 180;
}

DataLayer::~DataLayer()
{
	//移除自定义监听器
	_eventDispatcher->removeCustomEventListeners("ScoreChange");
}

//初始化datalayer层，包括了里面的精灵、菜单和文字等内容
bool DataLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//左上角的计分(也就是显示你多重)框框
	auto scoreBackground = Sprite::create("GameScene/score_background.png");
	scoreBackground->setPosition(Vec2(origin.x + scoreBackground->getContentSize().width / 2,
		origin.y + visibleSize.height - scoreBackground->getContentSize().height));
	this->addChild(scoreBackground, 0);

	//然后把分数在框框上展示
	score_label = Label::createWithTTF("", "fonts/arial.ttf", 14);
	score_label->setPosition(Vec2(origin.x + scoreBackground->getContentSize().width / 2,
		origin.y + visibleSize.height - scoreBackground->getContentSize().height));
	this->addChild(score_label, 1);

	//中间显示时间的框框
	auto timeBackground = Sprite::create("GameScene/score_background.png");
	timeBackground->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - timeBackground->getContentSize().height));
	this->addChild(timeBackground, 0);

	//屏幕上方中央的时间标签
	time_label = Label::createWithTTF("", "fonts/arial.ttf", 18);
	time_label->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - timeBackground->getContentSize().height));
	this->addChild(time_label, 1);


	_eventDispatcher->addCustomEventListener("ScoreChange", CC_CALLBACK_1(DataLayer::updateScore, this));
	this->schedule(schedule_selector(DataLayer::updateTime), 1);

	return true;
}

//更新玩家分数
void DataLayer::updateScore(EventCustom *event)
{
	int *score = (int*)(event->getUserData());  //收到事件分发时设置的UserData(此处即分数 int*)
	score_label->setString(StringUtils::format("Score: %d", *score));
	SCORE = *score;
}

//更新时间(不如说是同步时间)
void DataLayer::updateTime(float dt)
{
	_time -= dt;
	int minute = _time / 60;
	int second = _time % 60;
	if(second<10)
		time_label->setString(StringUtils::format("%d : 0%d", minute, second));
	else
		time_label->setString(StringUtils::format("%d : %d", minute, second));

	if (_time <= 0)
	{
		SceneManager::getInstance()->changeScene(SceneManager::en_GameOverScene);
	}
}

void DataLayer::onExit()
{
	Layer::onExit();
}