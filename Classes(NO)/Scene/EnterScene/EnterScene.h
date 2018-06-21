#ifndef EnterScene_h
#define EnterScene_h

#include "cocos2d.h"
USING_NS_CC;

class EnterScene : public Layer
{
public:
	static Scene * createScene();
	virtual bool init();
	EnterScene();
	~EnterScene();

private:
	//float dt不要动！！千万不要动！！！
	void enterMenuScene(float dt);      //进入菜单场景


	CREATE_FUNC(EnterScene);
};
#endif /* EnterScene_h */
