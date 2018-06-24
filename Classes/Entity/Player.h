#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "cocos2d.h"
#include "PlayerDivision.h"

USING_NS_CC;

class Bean;
class Spore;
class Prick;
class PlayerDivision;


class Player : public Node
{
public:

	enum class State        //玩家的四种状态
	{
		NORMAL,             //正常状态
		DIVIDE,             //分身状态
		SPIT,               //吐球状态
		CONCENTRATE         //主动中合状态
	};

	Player() { division_num = 0; }
	~Player()
	{
		division_list.clear();//释放division_list里所有PlayerDivision指针指向的内存
	}


	static Player* create(Vec2  position, Node *map);                           //以分身位置与地图指针为参数创建Player并且同时创建一个初始分身
	static Player* create_ai(Vec2 position, Node *map);
	bool init(Vec2  position, Node *map);
	bool init_ai(Vec2 position, Node *map);

	static Player* create(std::string &playername, int skinID, Vec2 velocity, Node *map);//以用户名、皮肤图片序号、速度（？？？）、地图指针为参数创建Player
	bool init(std::string &playername, int skinID, Vec2 velocity, Node *map);

	PlayerDivision* createDivision(Vec2 position, Vec2 velocity, int score);    //创建一个玩家分身

	void dividePlayer();                                                        //分裂(所有分身一裂为二)
	void setCombine(float) { combine_bool = true; }
	void divideFinish() { _state = State::NORMAL; }

	int countSpitSporeNum();                                                    //计算玩家一次可以吐出几个孢子
	void spitSpore(int id, Map<int, Spore*> &sporeMap, int globalID);        //吐孢子(每个分身只要达到了特定分值都可以吐一个)

	bool collideBean(Bean *bean);                                               //对所有分身进行检验，只要有一个分身能吃这个豆豆，就吃了它！
	bool collideSpore(Spore *spore);                                            //对所有分身进行检验，只要有一个分身能吃这个孢子，就吃了它！
	bool collidePrick(Prick *prick);                                            //对所有分身检验，只要有一个分身与刺有效碰撞，那么在分身数量允许的情况下就裂了

	int collideDivision(PlayerDivision *pdivision);                             //玩家分身与对手分身的吃与被吃检验
	bool collideRival(Player *rival);                                           //检验后，对被吃的玩家分身进行清除


	virtual void onExit();

	void updateDivision();                                                      //玩家信息更新
	void resetPlayer();                                                         //玩家复活

	void concentrate();                                                         //玩家中合
	void setConcentrate() { _state = State::CONCENTRATE; }

	void setVelocity(Vec2 v);                                                   //设置速度

	Rect getPlayerRect();                                                       //获取玩家包围盒
	float getTotalWeight();
	int getTotalScore();

	void clearDivisionList();

	Vector<PlayerDivision*>& getDivisionList() { return division_list; }
	int& getDivisionNum() { return division_num; }
	std::string getPlayerName() { return player_name; }
	Vec2 getVelocity() { return _velocity; }

private:
	Vector<PlayerDivision*> division_list;                                      //玩家分身列表
	int division_num;                                                           //分身数量

	Node *_map;                                                                 //地图指针
	std::string player_name = "Hello";                                                    //玩家名
	int skin_id;                                                                //关键字id

	Vec2 _velocity;                                                             //速度
	int _color;                                                                 //颜色,没有皮肤时可用

	State _state;                                                               //玩家状态
	bool combine_bool;                                                          //分身是否可以合体
};

#endif