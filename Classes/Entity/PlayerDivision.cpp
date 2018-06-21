#include "PlayerDivision.h"
#include "Bean.h"
#include "Spore.h"
#include "Prick.h"
#include "Header/Macros.h"

PlayerDivision* PlayerDivision::create(const std::string &picturename)
{
	PlayerDivision* playerDivision = new PlayerDivision();
	if (playerDivision && playerDivision->init(picturename))
	{
		playerDivision->autorelease();
		return playerDivision;
	}

	CC_SAFE_DELETE(playerDivision);
	return nullptr;
}

bool PlayerDivision::init(const std::string &picturename)
{
	if (!Entity::initWithFile(picturename))
	{
		return false;
	}

	return true;
}

PlayerDivision* PlayerDivision::create(const std::string &playername, int skinID, int s)
{
	PlayerDivision* playerDivision = new PlayerDivision();
	if (playerDivision && playerDivision->init(playername, skinID, s))
	{
		playerDivision->autorelease();
		return playerDivision;
	}

	CC_SAFE_DELETE(playerDivision);
	return nullptr;
}

bool PlayerDivision::init(const std::string &playername, int skinID, int s)
{
	skin_id = skinID;
	//if (!Entity::initWithFile(StringUtils::format("public/skin_%d.png", skin_id)))
	if (!Entity::initWithFile(StringUtils::format("public/keyword_%d.png", skin_id)))
	{
		return false;
	}

	_score = s;
	calculateData();

	return true;
}


void PlayerDivision::calculateData()
{
	_radius = sqrt(_score * PLAYER_INITIAL_RADIUS * PLAYER_INITIAL_RADIUS / PLAYER_INITIAL_SCORE);//sqtr():求平方根
	speed = (PLAYER_INITIAL_RADIUS / _radius) * (PLAYER_INITIAL_ISPEED - PLAYER_MIN_SPEED) + PLAYER_MIN_SPEED;

	this->setLocalZOrder(_score);     //分值大的球球后渲染
	scaleSprite();
}

void PlayerDivision::scaleSprite()
{
	Size size = this->getContentSize();
	float scale = float(_radius * 2) / size.width;

	this->setScale(scale);
}


bool PlayerDivision::collideBean(Bean *bean)
{
	Vec2 position = this->getPosition();
	Vec2 beanPosition = bean->getPosition();

	//如果豆豆和球的距离小于等于球半径，那么这个豆豆就被吃掉了！
	if (beanPosition.distance(position) <= _radius)
	{
		eatBean(bean);
		return true;
	}

	return false;
}

void PlayerDivision::eatBean(Bean *bean)
{
	_score += bean->getScore();
	calculateData();
}


bool PlayerDivision::collideSpore(Spore *spore)
{
	//如果球球的分值没有达到吃孢子的最小要求，就返回false
	if (_score < PLAYER_MIN_EAT_SPORE_SCORE)
	{
		return false;
	}

	Vec2 position = this->getPosition();
	Vec2 sporePosition = spore->getPosition();

	//如果孢子和球的距离小于等于球半径，那么这个孢子就被吃掉了！
	if (position.distance(sporePosition) <= _radius)
	{
		eatSpore(spore);
		return true;
	}

	return false;
}

void PlayerDivision::eatSpore(Spore *spore)
{
	_score += spore->getScore();
	calculateData();
}


bool PlayerDivision::collidePrick(Prick *prick)
{
	//如果球球够格引起这个绿刺的注意，那么球球就可能要被削了
	if (_score > (EAT_PRICK_DIVISION_NUM * SPORE_SCORE + prick->getScore()))
	{
		Vec2 prickPosition = prick->getPosition();
		Vec2 divisionPostion = this->getPosition();

		//如果绿刺完全进入球球内，那么返回true
		if (prickPosition.distance(divisionPostion) < (_radius + prick->getRadius()))
			return true;
	}

	return false;
}

//与刺有效碰撞后，球球的分值会变
void PlayerDivision::eatPrick(int s)
{
	_score = s;
	calculateData();
}


//吃掉别人的球球了？那么它的分值加给你的球球咯
void PlayerDivision::eatRival(int s)
{
	_score += s;
	calculateData();
}

//吐孢子，然后球球的分值就减少了，要重新计算本体的大小和速度
void PlayerDivision::spitSpore()
{
	_score -= SPORE_SCORE;
	calculateData();
}

void PlayerDivision::divide()
{
	_score /= 2;
	calculateData();
}

void PlayerDivision::setScore(int s)
{
	_score = s;
	calculateData();
}

void PlayerDivision::setVelocity(Vec2 v)
{
	velocity = v;
}

void PlayerDivision::setPrePosition(const Vec2 &position)
{
	pre_position = position;
}

void PlayerDivision::setPlayerName(const std::string &name)
{
	//创建玩家名标签
	name_label = Label::createWithTTF(name.c_str(), "fonts/Marker Felt.ttf", 22);
	//设置标签位置
	Size size = this->getContentSize();
	name_label->setPosition(Vec2(size.width / 2, size.height / 2));
	this->addChild(name_label);
}