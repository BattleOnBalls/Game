#ifndef _MENU_SCENE_H_
#define _MENU_SCENE_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class MenuScene : public Scene
{
public:
	static Scene* createScene();
	virtual bool init();
	CREATE_FUNC(MenuScene);

	void menuSkinCallback(Ref *pSender);
	void menuStartCallBack(Ref *pSender);

	virtual void onExit();

private:
	bool _nameSet;
};

#endif