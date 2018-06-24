#ifndef _MENU_SKIN_H_
#define _MENU_SKIN_H_

#include "cocos2d.h"
USING_NS_CC;

class MenuSkin : public Layer
{
public:
	static Scene* createScene();
	virtual bool init();
	CREATE_FUNC(MenuSkin);

	void menuSkinCallback1(Ref *pSender) { returnScene(1); }
	void menuSkinCallback2(Ref *pSender) { returnScene(2); }
	void menuSkinCallback3(Ref *pSender) { returnScene(3); }
	void menuSkinCallback4(Ref *pSender) { returnScene(4); }
	void menuSkinCallback5(Ref *pSender) { returnScene(5); }
	void menuSkinCallback6(Ref *pSender) { returnScene(6); }
	void menuBackCallback(Ref *pSender) { returnScene(0); }
	void returnScene(int skinID);

	virtual void onExit();
};

#endif