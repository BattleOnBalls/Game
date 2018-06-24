#ifndef _DATA_LAYER_H_
#define _DATA_LAYER_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

//数据图层，负责更新和显示玩家游戏时的数据

class DataLayer : public Layer
{
public:
	DataLayer();
	~DataLayer();

	virtual bool init();
	CREATE_FUNC(DataLayer);

	virtual void onExit();

	void updateTime(float dt);
	void updateScore(EventCustom *event);
private:
	Label *time_label;     //显示时间
	Label *score_label;     //显示分数
	int _time;     //时间
};

#endif