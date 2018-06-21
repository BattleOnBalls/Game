#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include "cocos2d.h"

USING_NS_CC;

/*游戏中使用的虚拟摇杆类*/

class Joystick : public Node {
public:
	Joystick();
	~Joystick();

	static Joystick * create(const std::string & backgroundPath, const std::string & thumbPath);

	virtual bool init(const std::string & backgroundPath, const std::string & thumbPath);

	bool onTouchBegan(Touch * touch, Event * event);
	void onTouchMoved(Touch * touch, Event * event);
	void onTouchEnded(Touch * touch, Event * event);
	void onTouchCancelled(Touch * touch, Event * event);

	Vec2 getVelocity();
	void updateVelocity(Vec2 point);

	//摇杆可控制的方向数量
	void setDirectionNum(int num);
	//中心不可触摸区域半径
	void setDeadRadius(float r);
	//取消触摸后控制杆自动居中
	void setAutoCenter(bool b);

	float round(float r);            //四舍五入
private:
	Sprite * _thumb;
	float _joystickRadius;
	float _joystickRadiusSq;  //背景半径平方
	float _thumbRadius;
	float _thumbRadiusSq;  //摇杆半径平方
	float _deadRadius;
	float _deadRadiusSq;    //不可触摸区域半径平方
	Vec2 _velocity;
	int _directionNum;
	bool _hasDead;
	bool _isDPad;           //有单位方向
	bool _autoCenter;        //自动居中
};



#endif