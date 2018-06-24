#ifndef _GAME_OVER_SCENE_H_
#define _GAME_OVER_SCENE_H_

#include "cocos2d.h"
USING_NS_CC;

class GameOverScene : public Scene
{
public:
	static Scene* createScene();
	virtual bool init();
	CREATE_FUNC(GameOverScene);


	void gameoverCloseCallback(Ref *pSender);
	void gameoverReturnCallback(Ref *pSender);
};

#endif