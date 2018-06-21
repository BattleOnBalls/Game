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

	void updateTime(EventCustom *event);
	void updateScore(EventCustom *event);
	//void updateRank(EventCustom *event);
private:
	Label * time_label;             //��ʾʱ��
	Label *score_label;             //��ʾ����
									//Vector<Label*> rank_list;     //���б�(���������ϱ����������У���ʵ����ֻ��ʾǰ����)
									//Scale9Sprite *rank1;
									//Scale9Sprite *rank2;
									//Scale9Sprite *rank3;
};

#endif