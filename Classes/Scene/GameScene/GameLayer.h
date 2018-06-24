#ifndef _GameLayer_H_
#define _GameLayer_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Tools/Joystick.h"

USING_NS_CC;
using namespace ui;

class Player;
class Bean;
class Spore;
class Prick;

class GameLayer : public Layer
{
public:
	GameLayer();
	~GameLayer();

	virtual bool init();
	virtual void onExit();
	CREATE_FUNC(GameLayer);


	bool onTouchBegan(Touch *touch, Event *event);
	void onTouchMoved(Touch *touch, Event *event);
	void onTouchEnded(Touch *touch, Event *event);
	void onTouchCancelled(Touch *touch, Event *event);


	void initBean();
	void initPlayer();
	void initRival();
	void initAi();
	void collide();
	void aiAction(float dt);
	Vec2 aiMoveVelocity(Vec2, Vec2);            //ai�ƶ��ٶ��趨
	void moveToBean();


	virtual void update(float dt);              //����
	void startAddPrick(float dt);               //�̿�ʼ����
	void addPrick(float dt);                    //�̲������
	void updateScore(float dt);                 //��ҷ�������

	void updateBean();                          //����С����
	void collideBean(Player *player);           //С������ײ���
	void resetBean(Node *node);                 //���ö���
	void updateSpore();                         //��������
	void updatePrick();                         //�����̴�
	void updateView();                          //�����Ϊ����������Ϸ��ͼ


	void dividePlayer();                        //�������
	void spitSpore();                           //�����Ӳ���

private:
	Joystick * _joystick;                        //����ҡ��
	Node *_map;                                 //��Ϸ��ͼ

	Vector<Bean *> _beanList;                   //�����б�
	Map<int, Spore *> _sporeMap;                //����
	Map<int, Prick *> _prickMap;                //�̴�
	//std::vector<int> _vecSporeNeedUpdate;       //��Ҫ���µ������б�

	Map<std::string, Player *> _rivalMap;       //��ͼ������б�

	Player *_player;                            //��Ϸ���
	Player *_ai;                                //computer ai

	Vec2 player_position;

	Vec2 ai_position;

	Vec2 player_move_direction;
	float _mapScale;                            //��ͼ��������
};

#endif