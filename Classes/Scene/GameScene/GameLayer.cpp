#include "GameLayer.h"
#include "Entity/Entity.h"
#include "Entity/Bean.h"
#include "Entity/Spore.h"
#include "Entity/Prick.h"
#include "Entity/PlayerDivision.h"
#include "Entity/Player.h"
#include "Scene/SceneManager/SceneManager.h"
#include "Macros.h"
#include <time.h>
#include <math.h>

#define SAFE_DISTANCE 500
#define MAX_DISTANCE 5000
#define MAX_SCORE_CHAZHI 50
#define LONG_DISTANCE 1000


enum GameLayerZOrder
{
	GAME_LAYER_BACKGROUND_Z,            //����
	GAME_LAYER_MAP_Z,                   //��ͼ
	GAME_LAYER_SPRITE_Z,                //����
	GAME_LAYER_JOYSTICK_Z,              //����ҡ�˵Ĳٿ�
};
GameLayer::GameLayer()
{

}

GameLayer::~GameLayer()
{
	_rivalMap.clear();
	_beanList.clear();

	_sporeMap.clear();
	_prickMap.clear();

	_eventDispatcher->removeCustomEventListeners("Divide");
	_eventDispatcher->removeCustomEventListeners("Spit");
}

bool GameLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	//���ñ�����ɫ
	auto background_color = LayerColor::create(Color4B(49, 49, 49, 255), DESIGN_SCREEN_WIDTH, DESIGN_SCREEN_HEIGHT);
	this->addChild(background_color, GAME_LAYER_BACKGROUND_Z);

	//������Ϸ��ͼ
	_map = Node::create();
	_map->setContentSize(Size(MAP_WIDTH, MAP_HEIGHT));
	this->addChild(_map, GAME_LAYER_MAP_Z);

	//��ʼ������
	initBean();
	initAi();
	initPlayer();
	initRival();


	//��������ҡ��
	_joystick = Joystick::create("GameScene/base.png", "GameScene/joystick.png");
	_joystick->setPosition(Vec2::ZERO);
	_joystick->setVisible(false);
	this->addChild(_joystick, GAME_LAYER_JOYSTICK_Z);

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(GameLayer::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(GameLayer::onTouchEnded, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(GameLayer::onTouchCancelled, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);


	_eventDispatcher->addCustomEventListener("Divide", CC_CALLBACK_0(GameLayer::dividePlayer, this));
	_eventDispatcher->addCustomEventListener("Spit", CC_CALLBACK_0(GameLayer::spitSpore, this));


	//������ʱ��
	this->scheduleUpdate();  //ÿ֡Ĭ��ִ��һ��Update������this�ǵ�ǰ�ڵ㣬��layer
	this->scheduleOnce(schedule_selector(GameLayer::startAddPrick), 3);  //3.0��ִ��һ��
	this->schedule(schedule_selector(GameLayer::updateScore), 1);  //ÿ��1.0fִ��һ��
	this->schedule(schedule_selector(GameLayer::aiAction), 1);  //ÿ��1.0fִ��һ��
	//this->schedule(schedule_selector(GameLayer::alert), 3);  //ÿ��1.0fִ��һ��

	return true;
}

void GameLayer::initBean()
{
	srand(time(nullptr));

	for (int i = 0; i < MAP_DIVISION_Y; i++)
	{
		for (int j = 0; j < MAP_DIVISION_X; j++)
		{
			for (int m = 0; m < MAP_DIVISION_BEAN_NUM; m++)
			{
				int type = rand() % 4 + 3;  //������״�������� �ı��� �����
				int color = rand() % 6 + 1;  //������ɫ
				float r1 = rand_0_1();
				float r2 = rand_0_1();

				std::string path = StringUtils::format("bean_polygon%d_%d.png", type, color);
				auto bean = Bean::create(path.c_str());
				bean->setPosition(Vec2(DESIGN_SCREEN_WIDTH * (j + r1), DESIGN_SCREEN_HEIGHT * (i + r2)));
				bean->setLocalZOrder(bean->getScore());  //����ֵ������Ⱦ˳��
				_map->addChild(bean);
				_beanList.pushBack(bean);
			}
		}
	}
}

void GameLayer::initAi()
{
	srand(time(nullptr) + 3);
	float x = rand() % MAP_WIDTH;
	float y = rand() % MAP_HEIGHT;
	//log("��ʼ��aiλ��: ( %lf, %lf )\n", x, y);
	_ai = Player::create_ai(Vec2(x, y), _map);
	_ai->setLocalZOrder(_ai->getTotalScore());
	_map->addChild(_ai);
	_ai->updateDivision();
}

Vec2 GameLayer::aiMoveVelocity(Vec2 v1, Vec2 v2)
{
	double x = v1.x - v2.x;
	double y = v1.y - v2.y;

	double radians = atan2(y, x);
	//log("radians = %lf", radians);

	Vec2 v;
	v.x = cos(radians);
	v.y = sin(radians);

	//log("ai���ƶ��ٶ�Ϊ:(%lf, %lf��", v.x, v.y);
	return v;
}

void GameLayer::aiAction(float dt)
{
	Vector<PlayerDivision *> player_divisionlist = _player->getDivisionList();
	for (auto player_division : player_divisionlist)
	{
		player_position = player_division->getPosition();
		//log("���λ��: ( %lf, %lf )", player_position.x, player_position.y);
		break;
	}

	Vector<PlayerDivision *> ai_divisionlist = _ai->getDivisionList();
	for (auto ai_division : ai_divisionlist)
	{
		ai_position = ai_division->getPosition();
		//log("aiλ��: ( %lf, %lf )", ai_position.x, ai_position.y);
		break;
	}

	double distance = ccpDistance(player_position, ai_position);
	//log("ai��player֮��ľ���Ϊ: %lf", distance);

	int player_score = _player->getTotalScore();
	//log("��ҷ���Ϊ��%d", player_score);
	int ai_score = _ai->getTotalScore();
	//log("ai����Ϊ��%d", ai_score);
	if(distance < SAFE_DISTANCE)
	{
		if(ai_score < player_score)
		{
			//����С����ң�����Զ����ҷ����ƶ�
			_ai->setVelocity(aiMoveVelocity(ai_position, player_position));
		}
		else if(ai_score / 2 > player_score && ai_score > PLAYER_MIN_DIVIDE_SCORE)
		{
			//����������Ҳ��ҿ��Է���������ҷ����ƶ�
			_ai->setVelocity(ccpNeg(aiMoveVelocity(ai_position, player_position)));
			double distance = ccpDistance(player_position, ai_position);
			if(distance < PLAYER_MIN_DIVIDE_DISTANCE)
			{
				_ai->dividePlayer();
			}
		}
		else
		{
			moveToBean();
		}
	}
	else if(ai_score - player_score > MAX_SCORE_CHAZHI)
	{
		//����Զ������Ҳ��ҿ��Է���������ҷ����ƶ�
		_ai->setVelocity(ccpNeg(aiMoveVelocity(ai_position, player_position)));
		double distance = ccpDistance(player_position, ai_position);
		if(distance < PLAYER_MIN_DIVIDE_DISTANCE && ai_score / 2 > player_score)
		{
			_ai->dividePlayer();
		}
	}
	else
	{
		moveToBean();
	}
}

void GameLayer::initPlayer()
{
	srand(time(nullptr));
	float x = rand() % MAP_WIDTH;
	float y = rand() % MAP_HEIGHT;
	//log("��ʼ��������λ��: ( %lf, %lf )\n", x, y);
	_player = Player::create(Vec2(x, y), _map);
	_player->setLocalZOrder(_player->getTotalScore());
	_map->addChild(_player);

	_player->updateDivision();
	updateView();
}

void GameLayer::initRival()
{
	_rivalMap.insert(_ai->getPlayerName(), _ai);
}

void GameLayer::update(float dt)
{

	updateBean();
	updatePrick();
	updateSpore();
	_player->updateDivision();
	_ai->updateDivision();
	//alert();
	//aiAction();
	//updateRival();        //ÿ�������Ϣ����ҿͻ����Լ�����

	updateView();
	collide();

}

void GameLayer::startAddPrick(float dt)
{
	this->schedule(schedule_selector(GameLayer::addPrick), 5);
}

void GameLayer::addPrick(float dt)
{
	static int id = 0;
	Prick *prick = Prick::create("GameScene/prick.png");
	srand(time(nullptr));
	int xPosition = rand() % MAP_WIDTH;
	int yPosition = rand() % MAP_HEIGHT;
	prick->setPosition(Vec2(xPosition, yPosition));
	prick->setLocalZOrder(prick->getScore());
	_map->addChild(prick);
	_prickMap.insert(id, prick);
	id++;
}

//���·���
void GameLayer::updateScore(float dt)
{
	int score = _player->getTotalScore();
	_eventDispatcher->dispatchCustomEvent("ScoreChange", &score);
}

//����С����
void GameLayer::updateBean()
{
	collideBean(_player);

	for (auto item : _rivalMap)
	{
		auto rival = item.second;
		if (rival != NULL)
		{
			collideBean(rival);
		}
	}
}

void GameLayer::moveToBean()
{
	Vec2 pos1 = _ai->getPosition(), pos = Vec2::ZERO;
	int distance = MAX_DISTANCE;

	for (int index = 0; index<_beanList.size(); index++)
	{

		auto bean = _beanList.at(index);
		if (bean->isVisible())
		{
			Vec2 pos2 = bean->getPosition();
			//log("distance = %lf", pos1.getDistance(pos2));
			if (pos1.getDistance(pos2) < distance)
			{
				pos = pos2;
			}
		}

		index++;
	}
	_ai->setVelocity(ccpNeg(aiMoveVelocity(ai_position, pos)));
}


//�붹����ײ���
void GameLayer::collideBean(Player *player)
{
	Rect rect = player->getPlayerRect();
	Vec2 position = player->getPosition();

	Vec2 point1 = Vec2(rect.origin.x - rect.size.width / 2, rect.origin.y - rect.size.height / 2);//������½�
	int divisionX1 = floor(point1.x / DESIGN_SCREEN_WIDTH);
	int divisionY1 = floor(point1.y / DESIGN_SCREEN_HEIGHT);
	int division1 = divisionY1 * MAP_DIVISION_X + divisionX1;
	Vec2 point2 = Vec2(rect.origin.x + rect.size.width / 2, rect.origin.y + rect.size.height / 2);//������Ͻ�
	int divisionX2 = floor(point2.x / DESIGN_SCREEN_WIDTH);
	int divisionY2 = floor(point2.y / DESIGN_SCREEN_HEIGHT);
	int division2 = divisionY2 * MAP_DIVISION_X + divisionX2;

	//��������������
	if (divisionX1 < 0)
	{
		divisionX1 = 0;
	}
	if (divisionY1 < 0)
	{
		divisionY1 = 0;
	}
	if (divisionX2 == MAP_DIVISION_X)
	{
		divisionX2 = MAP_DIVISION_X - 1;
	}
	if (divisionY2 == MAP_DIVISION_Y)
	{
		divisionY2 = MAP_DIVISION_Y - 1;
	}
	int xSpan = divisionX2 - divisionX1;
	int ySpan = divisionY2 - divisionY1;
	int max_ball = MAP_DIVISION_BEAN_NUM;

	for (int i = 0; i <= ySpan; i++)
	{
		for (int j = 0; j <= xSpan; j++)
		{
			division1 = (divisionY1 + i)*MAP_DIVISION_X + divisionX1 + j;
			for (int index = division1 * max_ball, m = 0; m < max_ball; m++)
			{
				auto bean = _beanList.at(index);
				if (bean->isVisible())
				{
					if (player->collideBean(bean))
					{
						bean->setVisible(false);
						float time = rand() % 10 + 10;
						auto sequence = Sequence::create(
							DelayTime::create(time),
							CallFuncN::create(CC_CALLBACK_0(GameLayer::resetBean, this, bean)),
							NULL);
						bean->runAction(sequence);
						//return;
					}
				}
				index++;
			}
		}
	}
}

void GameLayer::resetBean(Node * node)
{
	node->setVisible(true);
}

//�������ӣ�������ĸ����ӱ��Ե���
void GameLayer::updateSpore()
{
	std::vector<int> vecDel;
	for (auto sporeItem : _sporeMap)
	{
		auto spore = sporeItem.second;
		if (spore != NULL)
		{
			Vec2 position = spore->getPosition();
			if (position.x < SPORE_RADIUS)
			{
				position.x = SPORE_RADIUS;
			}
			if (position.x > MAP_WIDTH - SPORE_RADIUS)
			{
				position.x = MAP_WIDTH - SPORE_RADIUS;
			}
			if (position.y < SPORE_RADIUS)
			{
				position.y = SPORE_RADIUS;
			}
			if (position.y > MAP_HEIGHT - SPORE_RADIUS)
			{
				position.y = MAP_HEIGHT - SPORE_RADIUS;
			}
			spore->setPosition(position);

			if (_player->collideSpore(spore) || _ai->collideSpore(spore))
			{
				vecDel.push_back(sporeItem.first);
			}
		}
	}
	for (auto key : vecDel)
	{
		auto spore = _sporeMap.at(key);
		_sporeMap.erase(key);
		spore->removeFromParentAndCleanup(true);
	}

	vecDel.clear();
}

//�����̴��Ƿ�ᱻ�ԣ���Ļ�����ʧ
void GameLayer::updatePrick()
{
	std::vector<int> vecDel;
	for (auto prickItem : _prickMap)
	{
		auto prick = prickItem.second;
		if (prick != NULL)
		{
			if (_player->collidePrick(prick) || _ai->collidePrick(prick))
			{
				vecDel.push_back(prickItem.first);
			}
		}
	}
	for (auto key : vecDel)
	{
		auto prick = _prickMap.at(key);
		_prickMap.erase(key);
		prick->removeFromParentAndCleanup(true);
	}
	vecDel.clear();
}



//�����Ϊ����������Ϸ��ͼ
void GameLayer::updateView()
{
	//auto rect = _ai->getPlayerRect();
	auto rect = _player->getPlayerRect();

	float scaleX = DESIGN_SCREEN_WIDTH / (DESIGN_SCREEN_WIDTH + rect.size.width);
	float scaleY = DESIGN_SCREEN_HEIGHT / (DESIGN_SCREEN_HEIGHT + rect.size.height);
	_mapScale = (scaleX < scaleY) ? scaleX : scaleY;

	float dx = rect.origin.x*_mapScale - DESIGN_SCREEN_WIDTH / 2;
	float dy = rect.origin.y*_mapScale - DESIGN_SCREEN_HEIGHT / 2;

	Vec2 newPosition = Vec2(-dx, -dy);

	_map->setScale(_mapScale);
	_map->setPosition(newPosition);
}

//��ҷ��ѵ���Ӧ����
void GameLayer::dividePlayer()
{
	_player->dividePlayer();
}

//��������ӵ���Ӧ����
void GameLayer::spitSpore()
{
	static int globalID = 0;
	int sporeCount = _player->countSpitSporeNum();
	_player->spitSpore(SKIN, _sporeMap, globalID);
	globalID += sporeCount;
}

//��ײ���
void GameLayer::collide()
{
	if (_player->collideRival(_ai))
	{
		float playerDivisionCount = _player->getDivisionNum();
		if (playerDivisionCount == 0)
		{
			_player->resetPlayer();
		}

		float aiDivisionCount = _ai->getDivisionNum();
		if (aiDivisionCount == 0)
		{
			_ai->resetPlayer();
		}
	}
}

//�˳�
void GameLayer::onExit()
{
	this->unscheduleAllCallbacks();
	Layer::onExit();
}


/*****����ҡ��*****/
//����ң�еĲٿ�
bool GameLayer::onTouchBegan(Touch * touch, Event * event)
{
	auto position = touch->getLocation();

	_joystick->setPosition(position);
	_joystick->setVisible(true);

	_joystick->onTouchBegan(touch, event);

	return true;
}

void GameLayer::onTouchMoved(Touch * touch, Event * event)
{
	_joystick->onTouchMoved(touch, event);
	player_move_direction = _joystick->getVelocity();
	_player->setVelocity(_joystick->getVelocity());
	//log("move player velocity = (%lf, %lf)", player_move_direction.x, player_move_direction.y);
}

void GameLayer::onTouchEnded(Touch * touch, Event * event)
{
	_joystick->onTouchEnded(touch, event);
	_joystick->setVisible(false);
	Vec2 velocity = _joystick->getVelocity();
	if (velocity.x == 0 && velocity.y == 0)
	{
		_player->setConcentrate();
	}
}

void GameLayer::onTouchCancelled(Touch *touch, Event *event)
{
	_joystick->onTouchCancelled(touch, event);
}