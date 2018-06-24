#ifndef _DATA_LAYER_H_
#define _DATA_LAYER_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

//����ͼ�㣬������º���ʾ�����Ϸʱ������

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
	Label *time_label;     //��ʾʱ��
	Label *score_label;     //��ʾ����
	int _time;     //ʱ��
};

#endif