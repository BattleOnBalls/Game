#ifndef _SceneManager_H_
#define _SceneManager_H_

#include "cocos2d.h"
USING_NS_CC;

class SceneManager : public Ref
{
public:
	enum SceneType
	{
		//en_HelloWorld,       //������
		en_EnterScene,       //���볡��
		en_MenuScene,        //��������
		en_GameScene,        //��Ϸ����
		en_GameOverScene,     //��Ϸ����
	};

	static SceneManager* getInstance();
	virtual bool init();

	void changeScene(SceneType enSceneType);
private:
	static SceneManager * s_SceneManager;

};


#endif