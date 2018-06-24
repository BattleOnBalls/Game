#include "Player.h"
#include "Bean.h"
#include "Spore.h"
#include "Prick.h"
#include "PlayerDivision.h"
#include "Macros.h"
#include "Scene/GameScene/GameLayer.h"

Player* Player::create(Vec2 position, Node *map)
{
	Player *player = new Player();
	if (player && player->init(position, map))
	{
		player->autorelease();
		return player;
	}

	CC_SAFE_DELETE(player);
	return nullptr;
}
Player* Player::create_ai(Vec2 position, Node *map)
{
	Player *player = new Player();
	if (player && player->init_ai(position, map))
	{
		player->autorelease();
		return player;
	}

	CC_SAFE_DELETE(player);
	return nullptr;
}

bool Player::init(Vec2 position, Node *map)
{
	//����player������ʧ�ܷ���false
	if (!Node::init())
	{
		return false;
	}

	player_name = NAME;
	_map = map;
	skin_id = SKIN;
	_state = State::NORMAL;
	combine_bool = true;

	auto division = this->createDivision(position, Vec2::ZERO, PLAYER_INITIAL_SCORE);
	_map->addChild(division, PLAYER_INITIAL_SCORE);

	return true;
}
bool Player::init_ai(Vec2 position, Node *map)
{
	//����player������ʧ�ܷ���false
	if (!Node::init())
	{
		return false;
	}

	player_name = "AI";
	_map = map;
	skin_id = (SKIN + 1) % 7;
	_state = State::NORMAL;
	combine_bool = true;

	auto division = this->createDivision(position, Vec2::ZERO, PLAYER_INITIAL_SCORE);
	_map->addChild(division, PLAYER_INITIAL_SCORE);

	return true;
}

PlayerDivision* Player::createDivision(Vec2 position, Vec2 velocity, int score)
{
	auto division = PlayerDivision::create(player_name, skin_id, score);  //����һ����ҷ���
	division->setPosition(position);  //���÷����λ��
	division->setPlayerName(player_name);  //�����������ǩ
	division->setVelocity(velocity);  //�����ٶ�
									  //�������ķ���ӵ������б���
	division_list.pushBack(division);  //pushBack:��push_back֮ǰ���division�Ƿ�Ϊ��ָ��
	division_num++;

	return division;
}

Player* Player::create(std::string &playername, int skinID, Vec2 velocity, Node *map)
{
	Player *player = new Player();
	if (player && player->init(playername, skinID, velocity, map))
	{
		player->autorelease();
		return player;
	}

	CC_SAFE_DELETE(player);
	return nullptr;
}

bool Player::init(std::string &playername, int skinID, Vec2 velocity, Node *map)
{
	//����player������ʧ�ܷ���false
	if (!Node::init())
	{
		return false;
	}

	player_name = playername;
	skin_id = skinID;
	_velocity = velocity;
	_map = map;
	_state = State::NORMAL;
	combine_bool = true;

	return true;
}

void  Player::dividePlayer()
{
	bool bo = false;
	int counter = division_num;

	for (auto division = division_list.begin(); division != division_list.end(); division++)
	{
		//������������Ѿ��ﵽ��������������ô�˳�ѭ��
		if (division_num + 1 > PLAYER_MAX_DIVISION_NUM)
		{
			break;
		}

		int score = (*division)->getScore();
		//����÷���ﵽ����Ҫ��
		if (score >= PLAYER_MIN_DIVIDE_SCORE)
		{
			this->unscheduleAllCallbacks();  //ֹͣ���ж�ʱ��
			_state = State::DIVIDE;  //״̬��Ϊ����״̬
			combine_bool = false;

			bo = true;

			(*division)->divide();  //���÷���ķ�ֵ����
			float radius = (*division)->getRadius();  //��ȡ�÷���İ뾶
			int score = (*division)->getScore();  //��ȡ�÷���ķ�ֵ
			Vec2 position = (*division)->getPosition();  //��ȡ�÷����λ��
			auto newDivision = this->createDivision(position, _velocity, score);  //�÷������һ���·���
			_map->addChild(newDivision);  //���·�������ͼ

			float angle = _velocity.getAngle();  //����arctan y/x (�Ƕ�)
			Vec2 newPosition = Vec2((radius + PLAYER_MIN_DIVIDE_DISTANCE)* cosf(angle),
				(radius + PLAYER_MIN_DIVIDE_DISTANCE)* sinf(angle));
			//cosf(angle)����angle��cosֵ��sinf(angle)����angle��sinֵ

			auto sequence = Sequence::create
			(EaseOut::create(MoveBy::create(0.5f, newPosition), 1.8f),
				//EaseOut:�����ɿ����,Ƶ��Ϊ1.8
				//MoveBy:������(Ҳ�����·����ԭ�����λ��)�ƶ�newPosition��ʾ�ľ��룬����0.5s
				CallFunc::create(CC_CALLBACK_0(Player::divideFinish, this)), NULL);  //��״̬���NOMAL
			newDivision->runAction(sequence);

			division = division_list.begin();
		}
		counter--;
		if (counter <= 0)
			break;
	}
	if (bo)
	{
		//15s��ִ��Player::setCombine����1���ѵ�5s�������п��ܺϲ�
		this->scheduleOnce(schedule_selector(Player::setCombine), 15);
	}
}

int Player::countSpitSporeNum()
{
	int count = 0;
	for (auto division : division_list)
	{
		if (division != NULL && division->getScore() >= PLAYER_MIN_SPIT_SCORE)
		{
			count++;
		}
	}
	return count;
}

void Player::spitSpore(int id, Map<int, Spore*> &sporeMap, int globalID)
{
	for (auto division : division_list)
	{
		if (division != NULL)
		{
			int score = division->getScore();
			if (score >= PLAYER_MIN_SPIT_SCORE)
			{
				division->spitSpore();  //���������ķ�ֵ��ȥһ�����ӵķ�ֵ

										//����һ�����Ӳ���������
				auto spore = Spore::create(id);
				Vec2 position = division->getPosition();
				float angle = division->getVelocity().getAngle();
				float radius = division->getRadius();
				Vec2 sporePosition = Vec2(position.x + radius * cosf(angle) * 2,
					position.y + radius * sinf(angle) * 2);
				spore->setPosition(sporePosition);
				_map->addChild(spore, spore->getScore());  //�����Ӱ���ֵ��Ⱦ

				Vec2 newPosition = Vec2(PLAYER_MIN_SPIT_DISTANCE * cosf(angle),
					PLAYER_MIN_SPIT_DISTANCE * sinf(angle));
				auto action = MoveBy::create(0.5, newPosition);
				auto action2 = EaseOut::create(action, 1.8f);
				spore->runAction(action2);

				sporeMap.insert(globalID, spore);
				globalID++;
			}
		}
	}
}

bool Player::collideBean(Bean *bean)
{
	for (auto division : division_list)
	{
		if (division != NULL)
		{
			//���鶹���Ƿ���Ա���
			if (division->collideBean(bean))
			{
				division->setLocalZOrder(division->getScore());  //���˶�������ֵ���ӣ�����Ҫ���������������Ⱦ˳��
				return true;
			}
		}
	}
	return false;
}

bool Player::collideSpore(Spore *spore)
{
	for (auto division : division_list)
	{
		if (division != NULL)
		{
			//���������Ƿ���Ա���
			if (division->collideSpore(spore))  //����ǿ��Ա��ԵĻ����������ֻ���˷�
			{
				division->setLocalZOrder(division->getScore());  //�������ӣ���ֵ���ӣ�����Ҫ���������������Ⱦ˳��
				return true;
			}
		}
	}
	return false;
}

bool Player::collidePrick(Prick *prick)
{
	for (auto division : division_list)
	{
		if (division != NULL)
		{
			//������̴�����Ч��ײ
			if (division->collidePrick(prick))  //��û�и�ʲô��������ֻ�Ǽ���ǲ�����Ч��ײ
			{
				int prickScore = prick->getScore();
				int divisionScore = division->getScore();

				//�����Ƿ��Ѵ������������Ѵ��ֻ�ӷ�ֵ��δ����ӷ�ֵ������
				if (division_num + 1 > PLAYER_MAX_DIVISION_NUM)
				{
					int splitScore = prickScore + divisionScore;
					division->eatPrick(splitScore);
					return true;
				}
				else
				{
					int num = (division_num + EAT_PRICK_DIVISION_NUM <= PLAYER_MAX_DIVISION_NUM) ?
						EAT_PRICK_DIVISION_NUM : (PLAYER_MAX_DIVISION_NUM - division_num);
					int splitScore = prickScore + divisionScore - num * AFTER_PRICK_SCORE;
					division->eatPrick(splitScore);

					//ѭ���������ѳ���С��
					float angle = 0;
					float anglePerDivision = PI * 2 / EAT_PRICK_DIVISION_NUM;
					for (int i = 0; i < num; i++)
					{
						_state = State::DIVIDE;
						combine_bool = false;

						Vec2 velocity = Vec2(cosf(angle), sinf(angle));
						auto splitDivision = this->createDivision(division->getPosition(), velocity, AFTER_PRICK_SCORE);
						_map->addChild(splitDivision, AFTER_PRICK_SCORE);  //����ֵ�������ѳ���С�����Ⱦ˳��

						Vec2 newPosition = Vec2(PRICK_SPLIT_DISTANCE * cosf(angle), PRICK_SPLIT_DISTANCE * sinf(angle));
						auto sequence = Sequence::create(
							EaseOut::create(MoveBy::create(0.3f, newPosition), 1.8f),
							//EaseOut:�����ɿ����,Ƶ��Ϊ1.8
							//MoveBy:������(Ҳ���Ǳ��ѳ���С���ԭ�����λ��)�ƶ�newPosition��ʾ�ľ��룬����0.3s
							CallFunc::create(CC_CALLBACK_0(Player::setConcentrate, this)), NULL);
						//collidePrickFinish��״̬���CONCENTRATE
						splitDivision->runAction(sequence);

						//�ı���һ��С���ѳ�ʱ�ķ��䷽��
						angle += anglePerDivision;
					}
				}
				//15s��ִ��Player::setCombine����15s�������п��ܺϲ�
				this->scheduleOnce(schedule_selector(Player::setCombine), 15);
				return true;
			}
		}
	}

	return false;
}

int Player::collideDivision(PlayerDivision *pdivision)
{
	Vec2 playerPosition = pdivision->getPosition();
	float playerRadius = pdivision->getRadius();

	for (int i = 0; i < division_list.size(); i++)
	{
		auto division = division_list.at(i);
		Vec2 rivalPosition = division->getPosition();
		float rivalRadius = division->getRadius();

		float distance = playerPosition.distance(rivalPosition);
		//������߳�Ϊ������ϵ����ô��Ȼ��һ������һ���Ե�
		if (distance <= (playerRadius + rivalRadius) / 2)  //abs:ȡ����ֵ
		{
			int playerScore = pdivision->getScore();
			int rivalScore = division->getScore();

			if (playerScore > rivalScore)  //��ҷ���Ե��˶���
			{
				pdivision->eatRival(rivalScore);
				division_list.eraseObject(division);
				division->removeFromParentAndCleanup(true); //ɾ�����ڵ��еĵ�ǰ�ڵ㲢�������(�Ѹ÷���ӵ�ͼremove)
				return 1;
			}
			else if (rivalScore > playerScore)//���ֳԵ�����ҷ���
			{
				/*�޸�*/
				division->eatRival(playerScore);
				return 2;
			}
		}
	}
	return 0;
}

bool Player::collideRival(Player *rival)
{
	bool bo = false;

	for (int i = 0; i < division_list.size(); i++)
	{
		auto division = division_list.at(i);
		int result = rival->collideDivision(division);

		if (result == 2)  //��ҷ�����
		{
			division_list.eraseObject(division);  //���Եķ���ӷ����б���������ˣ�
			division->removeFromParentAndCleanup(true); //ɾ�����ڵ��еĵ�ǰ�ڵ㲢�������(�Ѹ÷���ӵ�ͼremove)
			division_num--;
			i--;
			bo = true;
		}
		else if (result == 1)  //��ҷ�����˵���
		{
			rival->getDivisionNum()--;
			bo = true;
		}
	}
	return bo;
}
Rect Player::getPlayerRect()
{
	float minX, minY, maxX, maxY;

	auto division = division_list.at(0);
	if (division != NULL)
	{
		Vec2 divisionPosition = division->getPosition();
		float radius = division->getRadius();
		minX = divisionPosition.x - radius;
		maxX = divisionPosition.x + radius;
		minY = divisionPosition.y - radius;
		maxY = divisionPosition.y + radius;
	}

	for (auto item : division_list)
	{

		if (item != NULL)
		{
			Vec2 divisionPosition = item->getPosition();
			float radius = item->getRadius();
			if (minX > divisionPosition.x - radius) { minX = divisionPosition.x - radius; }
			if (maxX <divisionPosition.x + radius) { maxX = divisionPosition.x + radius; }
			if (minY >divisionPosition.y - radius) { minY = divisionPosition.y - radius; }
			if (maxY < divisionPosition.y + radius) { maxY = divisionPosition.y + radius; }
		}
	}

	Rect rect;
	rect.size = Size(maxX - minX, maxY - minY);
	rect.origin = Vec2((minX + maxX) / 2, (minY + maxY) / 2);
	return rect;
}

void Player::updateDivision()
{
	if (_state == State::CONCENTRATE)
	{
		this->concentrate();
	}

	auto rect = this->getPlayerRect();

	/*�����ٶȸı�ÿ�������λ��*/
	for (auto division : division_list)
	{
		if (division != NULL)
		{
			float speed = division->getSpeed();
			if (_state == State::CONCENTRATE)
			{
				speed = PLAYER_CONCENTRATE_SPEED;
			}
			Vec2 velocity = division->getVelocity();
			float dx = velocity.x*speed;
			float dy = velocity.y*speed;
			Vec2 divisionVec = Vec2(dx, dy); //�����ƶ�����
			Vec2 oldPosition = division->getPosition();
			Vec2 newPosition = Vec2(oldPosition.x + divisionVec.x, oldPosition.y + divisionVec.y);
			if (_state != State::CONCENTRATE && _state != State::DIVIDE) //������������״̬��Ҳ���ڷ���״̬������������Ŀ�£
			{
				Vec2 centerVec = rect.origin - oldPosition; // ָ�����ķ���
				centerVec.normalize();
				centerVec.x = centerVec.x*PLAYER_CONCENTRATE_SPEED*0.2;
				centerVec.y = centerVec.y*PLAYER_CONCENTRATE_SPEED*0.2;
				Vec2 moveVec = Vec2(centerVec.x + divisionVec.x, centerVec.y + divisionVec.y);
				float angle = Vec2::angle(centerVec, divisionVec);
				float cosAngle = cosf(angle);

				if (cosAngle > 0 && cosAngle < 1) //ֻ����н�С��90�ȵ����
					newPosition = Vec2(oldPosition.x + moveVec.x, oldPosition.y + moveVec.y);
			}

			float radius1 = division->getRadius();
			if (!combine_bool)//���ں��塢����״̬������������ײ���
			{
				for (auto division2 : division_list)
				{
					if (division2 != NULL && division2 != division)
					{
						Vec2 position2 = division2->getPosition();
						float radius2 = division2->getRadius();
						float oldDistance = position2.distance(oldPosition);
						if (oldDistance <= radius1 + radius2)//�������ʱ���ܲ�����ײ�����
						{
							/*
							������Բ��Բ�������߷ֱ��������ƶ�
							*/
							float distance = ((radius1 + radius2) - oldDistance) / 2;
							Vec2 vec = oldPosition - position2;
							float angle = vec.getAngle();
							//newPosition = Vec2(oldPosition.x + cosf(angle), oldPosition.y + sinf(angle));
							//division2->setPosition(Vec2(position2.x - cosf(angle), position2.y - sinf(angle)));
							break;
						}
					}
				}
			}

			if (newPosition.x <= radius1)
			{
				newPosition.x = radius1;
			}
			if (newPosition.x >= MAP_WIDTH - radius1)
			{
				newPosition.x = MAP_WIDTH - radius1;
			}
			if (newPosition.y <= radius1)
			{
				newPosition.y = radius1;
			}
			if (newPosition.y >= MAP_HEIGHT - radius1)
			{
				newPosition.y = MAP_HEIGHT - radius1;
			}
			division->setPrePosition(oldPosition);
			division->setPosition(newPosition);
		}
	}
	if (!combine_bool)//����������ײ��⣬�ƶ������������ײ��ȡ���ƶ�
	{
		for (auto division1 : division_list)
		{
			if (division1 != NULL)
			{
				for (auto division2 : division_list)
				{
					if (division2 != NULL && division2 != division1)
					{
						Vec2 position1 = division1->getPosition();
						Vec2 position2 = division2->getPosition();
						float radius1 = division1->getRadius();
						float radius2 = division2->getRadius();
						float distance = position1.distance(position2);
						if (distance < radius1 + radius2)
						{
							Vec2 oldPosition1 = division1->getPrePosition();
							Vec2 oldPosition2 = division2->getPrePosition();
							float oldDistance = oldPosition1.distance(oldPosition2);
							float fixDistance = 0;
							if (oldDistance <= radius1 + radius2)
							{
								fixDistance = 2;
							}
							else
							{
								fixDistance = (radius1 + radius2 - distance);
							}

							Vec2 v = position2 - position1;
							float angle = v.getAngle();
							Vec2 newPosition1 = Vec2(position1.x - cosf(angle)*fixDistance, position1.y - sinf(angle)*fixDistance);
							Vec2 newPosition2 = Vec2(position2.x + sinf(angle)*fixDistance, position2.y + sinf(angle)*fixDistance);
							division1->setPosition(newPosition1);
							//division2->setPosition(newPosition2);
							//break;
						}
					}
				}
			}
		}
	}

	if (combine_bool)  //����������
	{
		for (int i = 0; i < division_list.size(); i++)
		{
			auto division1 = division_list.at(i);
			if (division1 != NULL)
			{
				Vec2 position1 = division1->getPosition();
				float radius1 = division1->getRadius();
				for (int j = i + 1; j < division_list.size(); j++)
				{
					auto division2 = division_list.at(j);
					if (division2 != NULL)
					{
						Vec2 position2 = division2->getPosition();
						float radius2 = division2->getRadius();
						float distance = position2.distance(position1);
						if (distance < abs(radius2 - radius1))     //����������Ժ���
						{
							division_num--;

							combine_bool = false;
							this->scheduleOnce(schedule_selector(Player::setCombine), 8);

							if (radius1 > radius2)
							{
								division1->eatRival(division2->getScore());
								division_list.eraseObject(division2);
								division2->removeFromParentAndCleanup(true);
								return;
							}
							else
							{
								division2->eatRival(division1->getScore());
								division_list.eraseObject(division1);
								division1->removeFromParentAndCleanup(true);
								return;
							}
						}
					}

				}
			}
		}
	}
}

void Player::setVelocity(Vec2 v)
{
	_state = State::NORMAL;

	for (auto division : division_list)
	{
		if (division != NULL)
		{
			division->setVelocity(v);
		}
	}
	_velocity = v;
}

void Player::concentrate()
{
	auto rect = this->getPlayerRect();
	for (auto division : division_list)
	{
		if (division != NULL)
		{
			Vec2 divisionPosition = division->getPosition();
			Vec2 velocity = rect.origin - divisionPosition;
			velocity.normalize();
			division->setVelocity(velocity);
		}
	}
}

float Player::getTotalWeight()
{
	float weight = 0;
	for (auto division : division_list)
	{
		if (division != NULL)
		{
			weight += division->getScore();
		}
	}
	return weight;
}

void Player::resetPlayer()
{
	srand(time(nullptr));
	int xPosition = rand() % MAP_WIDTH;
	int yPosition = rand() % MAP_HEIGHT;
	Vec2 position = Vec2(xPosition, yPosition);

	auto division = this->createDivision(position, Vec2::ZERO, PLAYER_INITIAL_SCORE);
	_map->addChild(division, PLAYER_INITIAL_SCORE);

	_velocity = Vec2::ZERO;
}

int Player::getTotalScore()
{
	int score = 0;

	for (auto division : division_list)
	{
		if (division != NULL)
		{
			score += division->getScore();
		}
	}
	return score;
}

void Player::onExit()
{
	Node::onExit();
}

void Player::clearDivisionList()
{
	for (auto division : division_list)
	{
		division->removeFromParentAndCleanup(true);
	}
	division_list.clear();
	division_num = 0;
}