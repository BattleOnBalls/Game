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
	Vec2 aiMoveVelocity(Vec2, Vec2);            //ai移动速度设定
	void moveToBean();


	virtual void update(float dt);              //更新
	void startAddPrick(float dt);               //刺开始出现
	void addPrick(float dt);                    //刺不断添加
	void updateScore(float dt);                 //玩家分数更新

	void updateBean();                          //更新小豆豆
	void collideBean(Player *player);           //小豆豆碰撞检测
	void resetBean(Node *node);                 //重置豆子
	void updateSpore();                         //更新孢子
	void updatePrick();                         //更新绿刺
	void updateView();                          //以玩家为中心设置游戏地图


	void dividePlayer();                        //分身操作
	void spitSpore();                           //吐孢子操作

private:
	Joystick * _joystick;                        //虚拟摇杆
	Node *_map;                                 //游戏地图

	Vector<Bean *> _beanList;                   //豆子列表
	Map<int, Spore *> _sporeMap;                //孢子
	Map<int, Prick *> _prickMap;                //绿刺
	//std::vector<int> _vecSporeNeedUpdate;       //需要更新的孢子列表

	Map<std::string, Player *> _rivalMap;       //地图中玩家列表

	Player *_player;                            //游戏玩家
	Player *_ai;                                //computer ai

	Vec2 player_position;

	Vec2 ai_position;

	Vec2 player_move_direction;
	float _mapScale;                            //地图缩放因子
};

#endif