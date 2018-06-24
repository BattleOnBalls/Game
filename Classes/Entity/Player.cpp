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
	//创建player，创建失败返回false
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
	//创建player，创建失败返回false
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
	auto division = PlayerDivision::create(player_name, skin_id, score);  //创建一个玩家分身
	division->setPosition(position);  //设置分身的位置
	division->setPlayerName(player_name);  //创建玩家名标签
	division->setVelocity(velocity);  //设置速度
									  //将创建的分身加到分身列表中
	division_list.pushBack(division);  //pushBack:在push_back之前检查division是否为空指针
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
	//创建player，创建失败返回false
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
		//如果分身数量已经达到了最大分身数，那么退出循环
		if (division_num + 1 > PLAYER_MAX_DIVISION_NUM)
		{
			break;
		}

		int score = (*division)->getScore();
		//如果该分身达到分裂要求
		if (score >= PLAYER_MIN_DIVIDE_SCORE)
		{
			this->unscheduleAllCallbacks();  //停止所有定时器
			_state = State::DIVIDE;  //状态变为分身状态
			combine_bool = false;

			bo = true;

			(*division)->divide();  //将该分身的分值减半
			float radius = (*division)->getRadius();  //获取该分身的半径
			int score = (*division)->getScore();  //获取该分身的分值
			Vec2 position = (*division)->getPosition();  //获取该分身的位置
			auto newDivision = this->createDivision(position, _velocity, score);  //该分身产生一个新分身
			_map->addChild(newDivision);  //将新分身加入地图

			float angle = _velocity.getAngle();  //返回arctan y/x (角度)
			Vec2 newPosition = Vec2((radius + PLAYER_MIN_DIVIDE_DISTANCE)* cosf(angle),
				(radius + PLAYER_MIN_DIVIDE_DISTANCE)* sinf(angle));
			//cosf(angle)返回angle的cos值，sinf(angle)返回angle的sin值

			auto sequence = Sequence::create
			(EaseOut::create(MoveBy::create(0.5f, newPosition), 1.8f),
				//EaseOut:动作由快变慢,频率为1.8
				//MoveBy:将精灵(也就是新分身从原分身的位置)移动newPosition所示的距离，过程0.5s
				CallFunc::create(CC_CALLBACK_0(Player::divideFinish, this)), NULL);  //将状态设回NOMAL
			newDivision->runAction(sequence);

			division = division_list.begin();
		}
		counter--;
		if (counter <= 0)
			break;
	}
	if (bo)
	{
		//15s后执行Player::setCombine，即1分裂的5s后分身才有可能合并
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
				division->spitSpore();  //将这个分身的分值减去一个孢子的分值

										//创建一个孢子并且设置它
				auto spore = Spore::create(id);
				Vec2 position = division->getPosition();
				float angle = division->getVelocity().getAngle();
				float radius = division->getRadius();
				Vec2 sporePosition = Vec2(position.x + radius * cosf(angle) * 2,
					position.y + radius * sinf(angle) * 2);
				spore->setPosition(sporePosition);
				_map->addChild(spore, spore->getScore());  //将孢子按分值渲染

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
			//检验豆豆是否可以被吃
			if (division->collideBean(bean))
			{
				division->setLocalZOrder(division->getScore());  //吃了豆豆，分值增加，所以要重新设置球球的渲染顺序
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
			//检验孢子是否可以被吃
			if (division->collideSpore(spore))  //如果是可以被吃的话，这个操作只加了分
			{
				division->setLocalZOrder(division->getScore());  //吃了孢子，分值增加，所以要重新设置球球的渲染顺序
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
			//如果跟绿刺是有效碰撞
			if (division->collidePrick(prick))  //并没有干什么操作，这只是检测是不是有效碰撞
			{
				int prickScore = prick->getScore();
				int divisionScore = division->getScore();

				//检验是否已达最大分身数，已达就只加分值，未达则加分值并分裂
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

					//循环创建被裂出的小球
					float angle = 0;
					float anglePerDivision = PI * 2 / EAT_PRICK_DIVISION_NUM;
					for (int i = 0; i < num; i++)
					{
						_state = State::DIVIDE;
						combine_bool = false;

						Vec2 velocity = Vec2(cosf(angle), sinf(angle));
						auto splitDivision = this->createDivision(division->getPosition(), velocity, AFTER_PRICK_SCORE);
						_map->addChild(splitDivision, AFTER_PRICK_SCORE);  //按分值决定割裂出的小球的渲染顺序

						Vec2 newPosition = Vec2(PRICK_SPLIT_DISTANCE * cosf(angle), PRICK_SPLIT_DISTANCE * sinf(angle));
						auto sequence = Sequence::create(
							EaseOut::create(MoveBy::create(0.3f, newPosition), 1.8f),
							//EaseOut:动作由快变慢,频率为1.8
							//MoveBy:将精灵(也就是被裂出的小球从原分身的位置)移动newPosition所示的距离，过程0.3s
							CallFunc::create(CC_CALLBACK_0(Player::setConcentrate, this)), NULL);
						//collidePrickFinish将状态设成CONCENTRATE
						splitDivision->runAction(sequence);

						//改变下一个小球被裂出时的发射方向
						angle += anglePerDivision;
					}
				}
				//15s后执行Player::setCombine，即15s后分身才有可能合并
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
		//如果两者成为包含关系，那么必然有一方被另一方吃掉
		if (distance <= (playerRadius + rivalRadius) / 2)  //abs:取绝对值
		{
			int playerScore = pdivision->getScore();
			int rivalScore = division->getScore();

			if (playerScore > rivalScore)  //玩家分身吃掉了对手
			{
				pdivision->eatRival(rivalScore);
				division_list.eraseObject(division);
				division->removeFromParentAndCleanup(true); //删除父节点中的当前节点并清除动作(把该分身从地图remove)
				return 1;
			}
			else if (rivalScore > playerScore)//对手吃掉了玩家分身
			{
				/*修改*/
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

		if (result == 2)  //玩家分身被吃
		{
			division_list.eraseObject(division);  //被吃的分身从分身列表里清除掉了！
			division->removeFromParentAndCleanup(true); //删除父节点中的当前节点并清除动作(把该分身从地图remove)
			division_num--;
			i--;
			bo = true;
		}
		else if (result == 1)  //玩家分身吃了敌人
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

	/*根据速度改变每个分身的位置*/
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
			Vec2 divisionVec = Vec2(dx, dy); //分身移动方向
			Vec2 oldPosition = division->getPosition();
			Vec2 newPosition = Vec2(oldPosition.x + divisionVec.x, oldPosition.y + divisionVec.y);
			if (_state != State::CONCENTRATE && _state != State::DIVIDE) //不在主动合体状态，也不在分身状态，分身会往中心靠拢
			{
				Vec2 centerVec = rect.origin - oldPosition; // 指向中心方向
				centerVec.normalize();
				centerVec.x = centerVec.x*PLAYER_CONCENTRATE_SPEED*0.2;
				centerVec.y = centerVec.y*PLAYER_CONCENTRATE_SPEED*0.2;
				Vec2 moveVec = Vec2(centerVec.x + divisionVec.x, centerVec.y + divisionVec.y);
				float angle = Vec2::angle(centerVec, divisionVec);
				float cosAngle = cosf(angle);

				if (cosAngle > 0 && cosAngle < 1) //只处理夹角小于90度的情况
					newPosition = Vec2(oldPosition.x + moveVec.x, oldPosition.y + moveVec.y);
			}

			float radius1 = division->getRadius();
			if (!combine_bool)//不在合体、分身状态，分身会进行碰撞检测
			{
				for (auto division2 : division_list)
				{
					if (division2 != NULL && division2 != division)
					{
						Vec2 position2 = division2->getPosition();
						float radius2 = division2->getRadius();
						float oldDistance = position2.distance(oldPosition);
						if (oldDistance <= radius1 + radius2)//处理分身时可能产生碰撞的情况
						{
							/*
							将两个圆以圆心连接线分别向两边移动
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
	if (!combine_bool)//分身会进行碰撞检测，移动后如果产生碰撞则取消移动
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

	if (combine_bool)  //处理分身合体
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
						if (distance < abs(radius2 - radius1))     //两个分身可以合体
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