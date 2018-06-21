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

	virtual bool init();                                    //初始化
	virtual void onExit();                                          //退出
	virtual void update(float dt);                                  //更新
	void collide();                                         //碰撞检测

	bool onTouchBegan(Touch * touch, Event * event);
	void onTouchMoved(Touch * touch, Event * event);
	void onTouchEnded(Touch * touch, Event * event);
	void onTouchCancelled(Touch * touch, Event * event);

	void initDataDefault();                                 //单机调试初始化游戏数据
	void initBean();
	void initPlayer();
	void initRival();


	void initData();                                        //初始化游戏数据

	void initRival(rapidjson::Value &value);                //初始化对手
	void initPlayer(rapidjson::Value &value);               //初始化玩家
	void resetPlayer();                                     //重置玩家
	void updateView();                                      //以玩家为中心设置游戏地图

	void initBean(int seed);                                //初始化小豆豆
	void updateBean();                                      //更新小豆豆
	void resetBean(Node * node);                            //重置小豆豆
	void collideBean(Player * player);                      //小豆豆碰撞检测

	void initPrick(rapidjson::Value &value);                //初始化绿刺
	void addPrickEvent(EventCustom * event);                //加绿刺
	void updatePrick();                                     //更新绿刺

	void initSpore(rapidjson::Value &value);                //初始化孢子
	void spitSpore();                                       //吐孢子
	void synSporeInfo();                                    //同步孢子信息
	void updateSpore();                                     //更新孢子

	void spitSporeResultEvent(EventCustom * event);         //执行吐孢子操作

	void dividePlayer();                                    //向服务器发送玩家请求分身操作的信息
	void synPlayerInfo();                                   //同步玩家信息到服务器
	void synPlayerConcentrate();                            //同步玩家中和操作到服务器

	void playerDivideEvent(EventCustom * event);            //执行分身操作
	void playerConcentrateEvent(EventCustom * event);       //执行玩家中和操作
	void playerMoveEvent(EventCustom * event);              //执行玩家移动操作
	void updatePlayerEvent(EventCustom * event);            //更新玩家信息
	void enterPlayerEvent(EventCustom * event);             //新玩家进入事件
	void timeChange(float dt);                              //游戏时间更新

	void updateScore(float dt);                             //玩家分数更新
															//void updateRank(float dt);                            //排行榜更新
	CREATE_FUNC(GameLayer);

private:
	Joystick * _joystick;                                   //虚拟摇杆
	Node * _map;                                            //游戏地图
	Map<int, Spore *> _sporeMap;                             //孢子
	Map<int, Prick *> _prickMap;                            //绿刺
	Vector<Bean *> _beanList;                               //豆子列表
	Map<std::string, Player *> _rivalMap;                   //地图中玩家列表
	std::vector<int> _vecSporeNeedUpdate;                   //需要更新的孢子列表
	Player * _player;                                       //游戏玩家
	float _timeCount;                                       //游戏时间
	float _mapScale;                                        //地图缩放因子
};

#endif