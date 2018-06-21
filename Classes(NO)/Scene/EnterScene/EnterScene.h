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
	//float dt��Ҫ������ǧ��Ҫ��������
	void enterMenuScene(float dt);      //����˵�����


	CREATE_FUNC(EnterScene);
};
#endif /* EnterScene_h */
