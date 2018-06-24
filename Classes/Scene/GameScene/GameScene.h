#ifndef _GameScene_H_
#define _GameScene_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

class GameScene : public Scene
{
public:
	static Scene* createScene();   //��������

	virtual bool init();            //��ʼ��

	CREATE_FUNC(GameScene);         //�Զ��ڴ����


	void menuSpitCallback(Ref *pSender, CheckBox::EventType type);
	void menuDivideCallback(Ref *pSender, CheckBox::EventType type);
	void gameMenuCallback(Ref *pSender);
};
#endif