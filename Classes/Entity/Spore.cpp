#include "Spore.h"
#include "Macros.h"

Spore* Spore::create(const std::string &picturename)
{
	Spore *spore = new Spore();

	if (spore && spore->init(picturename))
	{
		spore->autorelease();
		return spore;
	}

	CC_SAFE_DELETE(spore);
	return nullptr;
}

bool Spore::init(const std::string &picturename)
{
	skin_id = 1;

	//创建Spore，创建失败返回false
	if (!Entity::initWithFile(picturename))
	{
		return false;
	}

	//将孢子的显示设置成规定大小
	_radius = SPORE_RADIUS;
	_score = SPORE_SCORE;
	Size size = this->getContentSize();
	float scale = (_radius * 2) / size.width;
	this->setScale(scale);

	return true;
}

Spore* Spore::create(int id)
{
	Spore *spore = new Spore();
	if (spore && spore->init(id))
	{
		spore->autorelease();
		return spore;
	}

	CC_SAFE_DELETE(spore);
	return nullptr;
}

bool Spore::init(int id)
{
	skin_id = id;
	std::string picturename = StringUtils::format("GameScene/skin_%d.png", id);

	//创建Spore，创建失败返回false
	if (!Entity::initWithFile(picturename))
	{
		return false;
	}

	_radius = SPORE_RADIUS;
	_score = SPORE_SCORE;
	Size size = this->getContentSize();
	float scale = (_radius * 2) / size.width;
	this->setScale(scale);

	return true;
}