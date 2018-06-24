#include "PlayerDivision.h"
#include "Bean.h"
#include "Spore.h"
#include "Prick.h"
#include "Macros.h"

PlayerDivision* PlayerDivision::create(const std::string &picturename)
{
	PlayerDivision *playerDivision = new PlayerDivision();

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

PlayerDivision* PlayerDivision::create(const std::string &playername, int skinID, int score)
{
	PlayerDivision *playerDivision = new PlayerDivision();
	if (playerDivision && playerDivision->init(playername, skinID, score))
	{
		playerDivision->autorelease();
		return playerDivision;
	}

	CC_SAFE_DELETE(playerDivision);
	return nullptr;
}

bool PlayerDivision::init(const std::string &playername, int skinID, int score)
{
	skin_id = skinID;

	if (!Entity::initWithFile(StringUtils::format("GameScene/skin_%d.png", skin_id)))
	{
		return false;
	}

	_score = score;
	calculateData();

	return true;
}


void PlayerDivision::calculateData()
{
	_radius = sqrt(_score * PLAYER_INITIAL_RADIUS * PLAYER_INITIAL_RADIUS / PLAYER_INITIAL_SCORE);//sqtr():��ƽ����
	speed = (PLAYER_INITIAL_RADIUS / _radius) * (PLAYER_INITIAL_ISPEED - PLAYER_MIN_SPEED) + PLAYER_MIN_SPEED;

	this->setLocalZOrder(_score);     //��ֵ����������Ⱦ
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

	//�����������ľ���С�ڵ�����뾶����ô��������ͱ��Ե��ˣ�
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
	//�������ķ�ֵ�ﵽ�����ӵ���СҪ��
	if (_score >= PLAYER_MIN_EAT_SPORE_SCORE)
	{
		Vec2 position = this->getPosition();
		Vec2 sporePosition = spore->getPosition();

		//������Ӻ���Ӵ�����ô������Ӿͱ��Ե��ˣ�
		if (position.distance(sporePosition) <= _radius + spore->getScore())
		{
			eatSpore(spore);  //�����ӵķּӹ���
			return true;
		}
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
	Vec2 prickPosition = prick->getPosition();
	Vec2 divisionPostion = this->getPosition();

	//�������ӽ����̴�
	if (prickPosition.distance(divisionPostion) < (_radius + prick->getRadius()))
	{
		//��ô�Ϳ����������������
		//������򹻸���������̴̵�ע�⣬��ô����Ϳ���Ҫ������
		if (_score > (EAT_PRICK_DIVISION_NUM * AFTER_PRICK_SCORE + PLAYER_INITIAL_SCORE))
			return true;
	}

	return false;
}

//�����Ч��ײ������ķ�ֵ���
void PlayerDivision::eatPrick(int score)
{
	_score = score;
	calculateData();
}


//�Ե����˵������ˣ���ô���ķ�ֵ�Ӹ��������
void PlayerDivision::eatRival(int score)
{
	_score += score;
	calculateData();
}

//�����ӣ�Ȼ������ķ�ֵ�ͼ����ˣ�Ҫ���¼��㱾��Ĵ�С���ٶ�
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

void PlayerDivision::setScore(int score)
{
	_score = score;
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

void PlayerDivision::setPlayerName(const std::string &playername)
{
	//�����������ǩ
	name_label = Label::createWithTTF(playername.c_str(), "fonts/Marker Felt.ttf", 40);
	//���ñ�ǩλ��
	Size size = this->getContentSize();
	name_label->setPosition(Vec2(size.width / 2, size.height / 2));
	this->addChild(name_label);
}