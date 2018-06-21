#ifndef _SceneManager_H_
#define _SceneManager_H_

#include "cocos2d.h"
USING_NS_CC;

/*场景管理器，负责各个场景的跳转*/

class SceneManager : public Ref
{
public:
	enum SceneType
	{
		//en_HelloWorld,       //测试用
		en_EnterScene,       //进入场景
		en_MenuScene,        //大厅场景
		en_GameScene,        //游戏场景
							 //en_GameOverScene,    //游戏结束
	};

	static SceneManager* getInstance();
	virtual bool init();

	void changeScene(SceneType enSceneType);
private:
	static SceneManager * s_SceneManager;

};


#endif