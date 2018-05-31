#include "Prick.h"
#include "Macros.h"

Prick* Prick::create(const std::string &picturename)
{
	Prick* prick = new Prick();
	if (prick && prick->init(picturename))
	{
		prick->autorelease();
		return prick;
	}

	CC_SAFE_DELETE(prick);
	return nullptr;
}

bool Prick::init(const std::string &picturename)
{
	//创建Prick，创建失败返回false
	if (!Entity::initWithFile(picturename))
	{
		return false;
	}

	//设置绿刺的大小
	_radius = PRICK_RADIUS;
	_score = PRICK_SCORE;
	Size size = this->getContentSize();
	float scale = (_radius * 2) / size.width;
	this->setScale(scale);

	return true;
}