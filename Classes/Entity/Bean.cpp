#include "Bean.h"
#include "Macros.h"

Bean::Bean()
{

}
Bean::~Bean()
{

}

Bean* Bean::create(const std::string &filename)
{
	Bean *bean = new Bean();

	if (bean && bean->init(filename))
	{
		//自动释放内存
		bean->autorelease();
		return bean;
	}

	CC_SAFE_DELETE(bean);
	return nullptr;
}

bool Bean::init(const std::string &filename)
{
	CCSpriteFrameCache * cache = CCSpriteFrameCache::sharedSpriteFrameCache();
	cache->addSpriteFramesWithFile("GameScene/bean.plist");
	//SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(filename);
	if (!Entity::initWithSpriteFrameName(filename))
	{
		return false;
	}


	//初始化分数和半径
	_score = BEAN_SCORE;
	_radius = BEAN_RADIUS;

	//获取当前物体实际大小
	Size size = this->getContentSize();

	//按照直径缩放
	float scale = (_radius * 2) / size.width;
	this->setScale(scale);

	return true;
}