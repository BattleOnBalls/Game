#ifndef _GameLayer_H_
#define _GameLayer_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Tools/Joystick.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"

USING_NS_CC;
using namespace ui;

class Player;
class Bean;
class Spore;
class Prick;

class GameLayer : public Layer {
public:
	GameLayer();
	~GameLayer();

	virtual bool init();                                    //��ʼ��
	virtual void onExit();                                          //�˳�
	virtual void update(float dt);                                  //����
	void collide();                                         //��ײ���

	bool onTouchBegan(Touch * touch, Event * event);
	void onTouchMoved(Touch * touch, Event * event);
	void onTouchEnded(Touch * touch, Event * event);
	void onTouchCancelled(Touch * touch, Event * event);

	void initDataDefault();                                 //�������Գ�ʼ����Ϸ����
	void initBean();
	void initPlayer();
	void initRival();


	void initData();                                        //��ʼ����Ϸ����

	void initRival(rapidjson::Value &value);                //��ʼ������
	void initPlayer(rapidjson::Value &value);               //��ʼ�����
	void resetPlayer();                                     //�������
	void updateView();                                      //�����Ϊ����������Ϸ��ͼ

	void initBean(int seed);                                //��ʼ��С����
	void updateBean();                                      //����С����
	void resetBean(Node * node);                            //����С����
	void collideBean(Player * player);                      //С������ײ���

	void initPrick(rapidjson::Value &value);                //��ʼ���̴�
	void addPrickEvent(EventCustom * event);                //���̴�
	void updatePrick();                                     //�����̴�

	void initSpore(rapidjson::Value &value);                //��ʼ������
	void spitSpore();                                       //������
	void synSporeInfo();                                    //ͬ��������Ϣ
	void updateSpore();                                     //��������

	void spitSporeResultEvent(EventCustom * event);         //ִ�������Ӳ���

	void dividePlayer();                                    //����������������������������Ϣ
	void synPlayerInfo();                                   //ͬ�������Ϣ��������
	void synPlayerConcentrate();                            //ͬ������кͲ�����������

	void playerDivideEvent(EventCustom * event);            //ִ�з������
	void playerConcentrateEvent(EventCustom * event);       //ִ������кͲ���
	void playerMoveEvent(EventCustom * event);              //ִ������ƶ�����
	void updatePlayerEvent(EventCustom * event);            //���������Ϣ
	void enterPlayerEvent(EventCustom * event);             //����ҽ����¼�
	void timeChange(float dt);                              //��Ϸʱ�����

	void updateScore(float dt);                             //��ҷ�������
															//void updateRank(float dt);                            //���а����
	CREATE_FUNC(GameLayer);

private:
	Joystick * _joystick;                                   //����ҡ��
	Node * _map;                                            //��Ϸ��ͼ
	Map<int, Spore *> _sporeMap;                             //����
	Map<int, Prick *> _prickMap;                            //�̴�
	Vector<Bean *> _beanList;                               //�����б�
	Map<std::string, Player *> _rivalMap;                   //��ͼ������б�
	std::vector<int> _vecSporeNeedUpdate;                   //��Ҫ���µ������б�
	Player * _player;                                       //��Ϸ���
	float _timeCount;                                       //��Ϸʱ��
	float _mapScale;                                        //��ͼ��������
};

#endif