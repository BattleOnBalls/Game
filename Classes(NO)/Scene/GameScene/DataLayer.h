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

	void updateTime(EventCustom *event);
	void updateScore(EventCustom *event);
	//void updateRank(EventCustom *event);
private:
	Label * time_label;             //显示时间
	Label *score_label;             //显示分数
									//Vector<Label*> rank_list;     //排行表(这是理论上保存的玩家排行，但实际上只显示前三名)
									//Scale9Sprite *rank1;
									//Scale9Sprite *rank2;
									//Scale9Sprite *rank3;
};

#endif