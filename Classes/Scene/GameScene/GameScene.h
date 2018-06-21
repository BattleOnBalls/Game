#ifndef _GameScene_H_
#define _GameScene_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

class GameScene : public Layer
{
public:
	static Scene * createScene();   //创建场景

	virtual bool init();            //初始化

	CREATE_FUNC(GameScene);         //自动内存管理


	void menuSpitCallback(Ref * pSender, CheckBox::EventType type);
	void menuDivideCallback(Ref * pSender, CheckBox::EventType type);
};
#endif