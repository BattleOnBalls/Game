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

	//������ͼƬ����create�����
	static PlayerDivision* create(const std::string &picturename);
	bool init(const std::string &picturename);

	//�����������(�û���)��Ƥ���š�score��create�����
	static PlayerDivision* create(const std::string &playername, int skinID, int score);
	bool init(const std::string &playername, int skinID, int score);

	void calculateData();     //���ݷ�ֵ��������뾶���ٶȣ����Ҹ��ݷ�ֵ�����������ʾ�ɶ��
	void scaleSprite();     //����������ʾ�Ĵ�С

	bool collideBean(Bean *bean);     //�������Բ��������������������͵���eatBean������true
	void eatBean(Bean *bean);     //�ӷ�ֵ����calculateData

	bool collideSpore(Spore *spore);     //�������Բ������ӵ������������͵���eatSpore������true
	void eatSpore(Spore *spore);     //�ӷ�ֵ����calculateData

	bool collidePrick(Prick *prick);     //������������Ч��ײ�������������ͷ���true
	void eatPrick(int score);     //����Чײ�̺󣬽�_score���ó�score

	void eatRival(int score);     //�Ե��˼ӷ�

	void spitSpore();     //�����Ӽ���
	void divide();     //������ķ�ֵ���벢��calculateData

	virtual void setScore(int score);     //���÷�ֵ
	void setVelocity(Vec2 v);     //
	void setPrePosition(const Vec2 &position);     //
	void setPlayerName(const std::string &playername);     //�����û���

	float getSpeed() { return speed; }
	Vec2 getVelocity() { return velocity; }
	Vec2 getPrePosition() { return pre_position; }

private:
	Label *name_label;     //�û���
	int skin_id;

	float speed;     //
	Vec2 velocity;     //
	Vec2 pre_position;     //��һ֡��λ��
};

#endif