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

	enum class State        //��ҵ�����״̬
	{
		NORMAL,             //����״̬
		DIVIDE,             //����״̬
		SPIT,               //����״̬
		CONCENTRATE         //�����к�״̬
	};

	Player() { division_num = 0; }
	~Player()
	{
		division_list.clear();//�ͷ�division_list������PlayerDivisionָ��ָ����ڴ�
	}


	static Player* create(Vec2  position, Node *map);                           //�Է���λ�����ͼָ��Ϊ��������Player����ͬʱ����һ����ʼ����
	static Player* create_ai(Vec2 position, Node *map);
	bool init(Vec2  position, Node *map);
	bool init_ai(Vec2 position, Node *map);

	static Player* create(std::string &playername, int skinID, Vec2 velocity, Node *map);//���û�����Ƥ��ͼƬ��š��ٶȣ�������������ͼָ��Ϊ��������Player
	bool init(std::string &playername, int skinID, Vec2 velocity, Node *map);

	PlayerDivision* createDivision(Vec2 position, Vec2 velocity, int score);    //����һ����ҷ���

	void dividePlayer();                                                        //����(���з���һ��Ϊ��)
	void setCombine(float) { combine_bool = true; }
	void divideFinish() { _state = State::NORMAL; }

	int countSpitSporeNum();                                                    //�������һ�ο����³���������
	void spitSpore(int id, Map<int, Spore*> &sporeMap, int globalID);        //������(ÿ������ֻҪ�ﵽ���ض���ֵ��������һ��)

	bool collideBean(Bean *bean);                                               //�����з�����м��飬ֻҪ��һ�������ܳ�����������ͳ�������
	bool collideSpore(Spore *spore);                                            //�����з�����м��飬ֻҪ��һ�������ܳ�������ӣ��ͳ�������
	bool collidePrick(Prick *prick);                                            //�����з�����飬ֻҪ��һ�����������Ч��ײ����ô�ڷ����������������¾�����

	int collideDivision(PlayerDivision *pdivision);                             //��ҷ�������ַ���ĳ��뱻�Լ���
	bool collideRival(Player *rival);                                           //����󣬶Ա��Ե���ҷ���������


	virtual void onExit();

	void updateDivision();                                                      //�����Ϣ����
	void resetPlayer();                                                         //��Ҹ���

	void concentrate();                                                         //����к�
	void setConcentrate() { _state = State::CONCENTRATE; }

	void setVelocity(Vec2 v);                                                   //�����ٶ�

	Rect getPlayerRect();                                                       //��ȡ��Ұ�Χ��
	float getTotalWeight();
	int getTotalScore();

	void clearDivisionList();

	Vector<PlayerDivision*>& getDivisionList() { return division_list; }
	int& getDivisionNum() { return division_num; }
	std::string getPlayerName() { return player_name; }
	Vec2 getVelocity() { return _velocity; }

private:
	Vector<PlayerDivision*> division_list;                                      //��ҷ����б�
	int division_num;                                                           //��������

	Node *_map;                                                                 //��ͼָ��
	std::string player_name = "Hello";                                                    //�����
	int skin_id;                                                                //�ؼ���id

	Vec2 _velocity;                                                             //�ٶ�
	int _color;                                                                 //��ɫ,û��Ƥ��ʱ����

	State _state;                                                               //���״̬
	bool combine_bool;                                                          //�����Ƿ���Ժ���
};

#endif