#ifndef _PLAYER_DIVISION_H_
#define _PLAYER_DIVISION_H_

#include "Entity.h"

class Bean;
class Spore;
class Prick;

class PlayerDivision : public Entity
{
public:
	PlayerDivision() { }
	~PlayerDivision() { }

	//用球球图片名称create这个球
	static PlayerDivision* create(const std::string &picturename);
	bool init(const std::string &picturename);

	//用球球的名字(用户名)、皮肤号、score来create这个球
	static PlayerDivision* create(const std::string &playername, int skinID, int s);
	bool init(const std::string &playername, int skinID, int s);

	void calculateData();     //根据分值计算球球半径、速度，并且根据分值决定球球该显示成多大
	void scaleSprite();     //设置球球显示的大小

	bool collideBean(Bean *bean);     //如果球球吃参数豆豆的条件成立，就调用eatBean并返回true
	void eatBean(Bean *bean);     //加分值并且calculateData

	bool collideSpore(Spore *spore);     //如果球球吃参数孢子的条件成立，就调用eatSpore并返回true
	void eatSpore(Spore *spore);     //加分值并且calculateData

	bool collidePrick(Prick *prick);     //如果球球与刺有效碰撞的条件成立，就返回true
	void eatPrick(int s);     //在有效撞刺后，将score设置成s

	void eatRival(int s);     //吃敌人加分

	void spitSpore();     //吐孢子减分
	void divide();     //将球球的分值减半并且calculateData

	virtual void setScore(int s);     //设置分值
	void setVelocity(Vec2 v);     //
	void setPrePosition(const Vec2 &position);     //
	void setPlayerName(const std::string &name);     //设置用户名

	float getSpeed() { return speed; }
	Vec2 getVelocity() { return velocity; }
	Vec2 getPrePosition() { return pre_position; }

private:
	Label *name_label;     //用户名
	int skin_id;     //有很多皮肤，那么这个球球用的是第几个呢？

	float speed;     //
	Vec2 velocity;     //
	Vec2 pre_position;     //上一帧的位置
};

#endif