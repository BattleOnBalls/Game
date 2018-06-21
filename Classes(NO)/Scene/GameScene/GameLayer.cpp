#include "GameLayer.h"
#include "Entity/Entity.h"
#include "Entity/Bean.h"
#include "Entity/Spore.h"
#include "Entity/Player.h"
#include "Entity/Prick.h"
#include "Entity/PlayerDivision.h"
#include "Scene/SceneManager/SceneManager.h"
#include "Header/Macros.h"
#include "Header/MessageType.h"
#include "Client/web.h"

#include "json/rapidjson.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/writer.h"


enum GameLayerZOrder
{
	GAME_LAYER_BACKGROUND_Z,            //背景
	GAME_LAYER_MAP_Z,                   //地图
	GAME_LAYER_SPRITE_Z,                //分裂
	GAME_LAYER_JOYSTICK_Z,              //虚拟摇杆的操控
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
	_eventDispatcher->removeCustomEventListeners("PlayerMove");
	_eventDispatcher->removeCustomEventListeners("PlayerDivide");
	_eventDispatcher->removeCustomEventListeners("SpitSporeResult");
	_eventDispatcher->removeCustomEventListeners("AddPrick");
	_eventDispatcher->removeCustomEventListeners("EnterPlayer");
	_eventDispatcher->removeCustomEventListeners("PlayerConcentrate");
	_eventDispatcher->removeCustomEventListeners("UpdatePlayer");

}

bool GameLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	//设置背景颜色
	auto background_color = LayerColor::create(Color4B(49, 49, 49, 255), DESIGN_SCREEN_WIDTH, DESIGN_SCREEN_HEIGHT);
	this->addChild(background_color, GAME_LAYER_BACKGROUND_Z);

	//设置游戏地图
	_map = Node::create();
	_map->setContentSize(Size(MAP_WIDTH, MAP_HEIGHT));
	this->addChild(_map, GAME_LAYER_MAP_Z);

	//初始化数据
	initData();
	//单机调试用
	//initDataDefault();

	//设置虚拟摇杆
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

	//EventDispatcher* eventDispatcher = Director::getInstance()->getEventDispatcher();

	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	_eventDispatcher->addCustomEventListener("Divide", CC_CALLBACK_0(GameLayer::dividePlayer, this));
	_eventDispatcher->addCustomEventListener("Spit", CC_CALLBACK_0(GameLayer::spitSpore, this));
	_eventDispatcher->addCustomEventListener("PlayerMove", CC_CALLBACK_1(GameLayer::playerMoveEvent, this));
	_eventDispatcher->addCustomEventListener("PlayerDivide", CC_CALLBACK_1(GameLayer::playerDivideEvent, this));
	_eventDispatcher->addCustomEventListener("SpitSporeResult", CC_CALLBACK_1(GameLayer::spitSporeResultEvent, this));
	_eventDispatcher->addCustomEventListener("AddPrick", CC_CALLBACK_1(GameLayer::addPrickEvent, this));
	_eventDispatcher->addCustomEventListener("EnterPlayer", CC_CALLBACK_1(GameLayer::enterPlayerEvent, this));
	_eventDispatcher->addCustomEventListener("PlayerConcentrate", CC_CALLBACK_1(GameLayer::playerConcentrateEvent, this));
	_eventDispatcher->addCustomEventListener("UpdatePlayer", CC_CALLBACK_1(GameLayer::updatePlayerEvent, this));


	//开启定时器
	this->scheduleUpdate();//每帧默认执行一次Update函数，this是当前节点，如layer
	this->schedule(schedule_selector(GameLayer::updateScore), 1);//每隔1.0f执行一次
	//this->schedule(schedule_selector(GameLayer::updateRank), 2);
	//this->schedule(schedule_selector(GameLayer::synPlayerInfo), 0.2);
	//this->schedule(schedule_selector(GameLayer::synPlayerMove), 0.1);
    //this->schedule(schedule_selector(GameLayer::synSporeInfo), 0.1);
    //this->scheduleOnce(schedule_selector(GameLayer::startAddPrick), 3);



	return true;
}

//初始化函数——单机调试用
void GameLayer::initDataDefault()
{
	initRival();
	initPlayer();
	initBean();
}

//初始化豆子——单机调试用
void GameLayer::initBean()
{
	for (int i = 0; i < MAP_DIVISION_Y; i++)
	{
		for (int j = 0; j < MAP_DIVISION_X; j++)
		{
			for (int m = 0; m < MAP_DIVISION_BEAN_NUM; m++)
			{

				int type = rand() % 4 + 3;
				int color = rand() % 6 + 1;
				float r1 = rand_0_1();
				float r2 = rand_0_1();

				std::string path = StringUtils::format("bean_polygon%d_%d.png", type, color);
				auto bean = Bean::create(path.c_str());
				bean->setPosition(Vec2(DESIGN_SCREEN_WIDTH*(j + r1), DESIGN_SCREEN_HEIGHT*(i + r2)));
				bean->setLocalZOrder(bean->getScore());
				_map->addChild(bean);
				_beanList.pushBack(bean);

			}

		}
	}
}

//初始化玩家——单机调试用
void GameLayer::initPlayer()
{
	float x = rand() % MAP_WIDTH;
	float y = rand() % MAP_HEIGHT;
	_player = Player::create(Vec2(x, y), _map);
	log("PLAYER POSITION : %f, %f", _player->getPosition().x, _player->getPosition().y);
	_player->setLocalZOrder(_player->getTotalScore());
	_map->addChild(_player);
	_player->updateDivision();
	updateView();

}
//初始化对手
void GameLayer::initRival()
{

	//_rivalMap.insert(_player->getPlayerName(), _player);

}


//初始化游戏数据！！！！！！！！！！！server
void GameLayer::initData()
{
	std::string config;
	cl.getConfig(config);

	if (config != "")
	{
		//生成Json串
		rapidjson::Document doc;                                                    //生成一个dom元素Document
		doc.Parse<0>(config.c_str());

		log("__________initData() Json: %s\n:", config.c_str());

		/*Json文件内容
		{
		"RoomSeed":        seed,
		"Player":
		{
		"GameName":        string类字符串,
		"KeywordID":       int整型数据（皮肤）,
		"vx":              double,
		"vy":              double,
		"DivisionList":
		{
		[x(double), y(double), score(int), vx(double), vy(double)]
		},
		},
		"RivalList":
		{
		"GameName":         string,
		"KeywordID":        int,
		"vx":               double,
		"vy":               double,
		"DivisionList":
		{
		[ x(double), y(double), score(int), vx(double), vy(double) ]
		},
		},
		"SporeList":       {
		[ gloablID(int), sporeID(int), x(double), y(double) ],
		},
		"PrickList":       {
		[ gloablID(int), x(int), y(int) ],
		},
		"PassTime":        double,

		//globalID：为地图中的全部spore和prick进行编号
		//sporeID：为单独一个玩家的spore进行编号
		}
		*/

		//判断解析从流中读取的字符串是否有错误
		if (doc.HasParseError())
		{
			log("json rerrorno: %d", doc.GetParseError());//打印错误编号
			return;
		}
		//确保它是一个Object
		if (doc.IsObject())
		{
			//获取json串中的数据
			//先判断是否有这个字段，如果使用不存在的key去取值会导致直接崩溃
			if (doc.HasMember("RoomSeed"))
			{
				int seed = doc["RoomSeed"].GetInt();                                //必须要获取对应的数据类型，rapidjson不会帮你转换类型
				initBean(seed);
			}
			if (doc.HasMember("Player"))
			{
				rapidjson::Value & value = doc["Player"];
				initPlayer(value);
			}
			if (doc.HasMember("RivalList"))
			{
				rapidjson::Value & value = doc["RivalList"];                        //创建一个Object类型的元素
				initRival(value);
			}
			if (doc.HasMember("SporeList"))
			{
				rapidjson::Value & value = doc["SporeList"];                        //创建一个Object类型的元素
				initSpore(value);
			}

			if (doc.HasMember("PrickList"))
			{
				rapidjson::Value & value = doc["PrickList"];                        //创建一个Object类型的元素
				initPrick(value);
			}
			if (doc.HasMember("PassTime"))
			{
				double passtime = doc["PassTime"].GetDouble();
				_timeCount = passtime;
				this->scheduleOnce(schedule_selector(GameLayer::timeChange), 0.1f);
			}
		}
	}
	else
	{
		log("Config = \"\"\n");
	}
}

//初始化对手
void GameLayer::initRival(rapidjson::Value &value)
{
	//获取数据总量
	int size = value.Size();
	//循环读取进行初始化
	for (int i = 0; i < size; i++)
	{
		std::string gameName = value[i]["GameName"].GetString();
		int keywordID = value[i]["KeywordID"].GetInt();
		double vx = value[i]["vx"].GetDouble();
		double vy = value[i]["vy"].GetDouble();
		auto player = Player::create(gameName, keywordID, Vec2(vx, vy), _map);
		_map->addChild(player);
		_rivalMap.insert(gameName, player);

		int divisionNum = value[i]["DivisionList"].Size();
		for (int j = 0; j < divisionNum; j++)
		{
			double x = value[i]["DivisionList"][j][0].GetDouble();
			double y = value[i]["DivisionList"][j][1].GetDouble();
			int score = value[i]["DivisionList"][j][2].GetInt();
			double vx = value[i]["DivisionList"][j][3].GetDouble();
			double vy = value[i]["DivisionList"][j][4].GetDouble();
			auto division = player->createDivision(Vec2(x, y), Vec2(vx, vy), score);
			_map->addChild(division, score);
		}
	}
}

//初始化玩家
void GameLayer::initPlayer(rapidjson::Value &value)
{
	//接收到的json数据格式为：
	/*Json文件内容
	{
	"GameName":        string类字符串,
	"KeywordID":       int整型数据（皮肤）,
	"vx":              double,
	"vy":              double,
	"DivisionList":    { [ x(double), y(double), score(int), vx(double), vy(double) ] },
	}
	*/
	std::string gameName = value["GameName"].GetString();
	int keywordID = value["KeywordID"].GetInt();
	double vx = value["vx"].GetDouble();
	double vy = value["vy"].GetDouble();
	_player = Player::create(gameName, keywordID, Vec2(vx, vy), _map);
	_map->addChild(_player);
	int divisionNum = value["DivisionList"].Size();

	for (int j = 0; j < divisionNum; j++)
	{
		double x = value["DivisionList"][j][0].GetDouble();
		double y = value["DivisionList"][j][1].GetDouble();
		int score = value["DivisionList"][j][2].GetInt();
		double vx = value["DivisionList"][j][3].GetDouble();
		double vy = value["DivisionList"][j][4].GetDouble();
		auto division = _player->createDivision(Vec2(x, y), Vec2(vx, vy), score);
		_map->addChild(division, score);
	}

	_player->updateDivision();
	updateView();
}

//初始化小豆豆
void GameLayer::initBean(int seed)
{
	//srand函数是随机数发生器的初始化函数
	srand(seed);
	for (int i = 0; i < MAP_DIVISION_Y; i++)
	{
		for (int j = 0; j < MAP_DIVISION_X; j++)
		{
			for (int m = 0; m < MAP_DIVISION_BEAN_NUM; m++)
			{
				int type = rand() % 4 + 3;
				int color = rand() % 6 + 1;
				float r1 = rand_0_1();
				float r2 = rand_0_1();

				std::string path = StringUtils::format("bean_polygon%d_%d.png", type, color);
				auto bean = Bean::create(path.c_str());

				bean->setPosition(Vec2(DESIGN_SCREEN_WIDTH*(j + r1), DESIGN_SCREEN_HEIGHT*(i + r2)));
				bean->setLocalZOrder(bean->getScore());
				_map->addChild(bean);
				_beanList.pushBack(bean);
			}

		}
	}
}

//初始化孢子
void GameLayer::initSpore(rapidjson::Value &value)
{
	//接收到的json数据格式为：
	/*Json文件内容
	{
	[ gloablID(int), sporeID(int), x(double), y(double) ]
	}
	*/
	int size = value.Size();
	for (int i = 0; i < size; i++)
	{
		int globalID = value[i][0].GetInt();
		int sporeID = value[i][1].GetInt();
		double x = value[i][2].GetDouble();
		double y = value[i][3].GetDouble();
		Spore * spore = Spore::create("public/keyword_1.png");
		spore->setPosition(x, y);
		_map->addChild(spore, spore->getScore());
		_sporeMap.insert(globalID, spore);
	}
}

//同步玩家信息**********client
void GameLayer::synPlayerInfo()
{
	rapidjson::Document doc;                                                        //生成一个dom元素Document
	doc.SetObject();                                                                //将当前的Document设置为一个object
	rapidjson::Document::AllocatorType & allocator = doc.GetAllocator();            //获取分配器

	rapidjson::Value divisionArr(rapidjson::kArrayType);                            //创建一个Array类型的元素

	Vector<PlayerDivision *> divisionList = _player->getDivisionList();
	Vec2 v = _player->getVelocity();

	for (auto division : divisionList)
	{
		rapidjson::Value arr(rapidjson::kArrayType);                                //创建一个Array类型的元素
		Vec2 position = division->getPosition();
		int score = division->getScore();
		/*
		json文件格式：
		{
		[x, y, score, vx, vy],
		}
		可用数组方法调用
		*/
		arr.PushBack(position.x, allocator);
		arr.PushBack(position.y, allocator);
		arr.PushBack(score, allocator);
		arr.PushBack(v.x, allocator);
		arr.PushBack(v.y, allocator);
		divisionArr.PushBack(arr, allocator);                                       ////将arr添加到数组divisionarr
	}

	std::string gameName = _player->getPlayerName();


	/*
	json文件格式：
	{
	"GameName":        gameName,
	"vx":              vx(double),
	"vy":              vy(double),
	"DivisionList":    { [x(double), y(double), score(int)] }
	"MsgType":         eMsg_UPDATE_POSITION,
	}
	*/

	doc.AddMember("GameName", rapidjson::Value(gameName.c_str(), allocator), allocator);
	doc.AddMember("vx", v.x, allocator);
	doc.AddMember("vy", v.y, allocator);
	doc.AddMember("DivisionList", divisionArr, allocator);
	doc.AddMember("MsgType", MessageType::eMsg_UPDATE_POSITION, allocator);

	//生成字符串
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> write(buffer);
	doc.Accept(write);

	std::string msg = buffer.GetString();

	log("__________synPlayerInfo() send Json:\n%s", msg.c_str());
	cl.sendMsg(buffer.GetString());


}

//向服务器发送信息同步孢子**********client
void GameLayer::synSporeInfo()
{
	rapidjson::Document doc;                                                        //生成一个dom元素Document
	doc.SetObject();                                                                //将当前的Document设置为一个object
	rapidjson::Document::AllocatorType & allocator = doc.GetAllocator();            //获取分配器

	rapidjson::Value arr(rapidjson::kArrayType);                                    //创建一个Array类型的元素

	if (_vecSporeNeedUpdate.size() == 0)                                            //没有需要更新的孢子，直接返回
	{
		return;
	}

	for (int i = 0; i < _vecSporeNeedUpdate.size(); i++)                            //for循环更新
	{
		int key = _vecSporeNeedUpdate[i];
		auto spore = _sporeMap.at(key);
		if (spore != NULL)
		{
			if (spore->getActionManager()->getNumberOfRunningActionsInTarget(spore))
			{
				Vec2 position = spore->getPosition();
				int id = spore->getID();

				rapidjson::Value tmpArr(rapidjson::kArrayType);                     //创建一个Array类型的元素
																					/*
																					json文件格式：
																					[key,id,x,y]
																					可用数组方法调用
																					*/
				tmpArr.PushBack(key, allocator);
				tmpArr.PushBack(id, allocator);
				tmpArr.PushBack(position.x, allocator);
				tmpArr.PushBack(position.y, allocator);
				arr.PushBack(tmpArr, allocator);                                     //将tmparr添加到数组arr
			}
			else
			{
				_vecSporeNeedUpdate.erase(_vecSporeNeedUpdate.begin() + i);         //更新后清除孢子，并使计数-1
				i--;
			}
		}
	}
	/*
	json文件格式：
	{
	"MsgType":     eMsg_UPDATE_SPORE,
	"SporeList":   { [key(int), id(int), x(double), y(double)] },
	}
	*/

	doc.AddMember("MsgType", MessageType::eMsg_UPDATE_SPORE, allocator);
	doc.AddMember("SporeList", arr, allocator);

	//生成字符串
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> write(buffer);
	doc.Accept(write);

	std::string msg = buffer.GetString();

	log("__________synSporeInfo() send Json:%s\n", msg.c_str());
	cl.sendMsg(buffer.GetString());
}

//吐孢子操作**********client
void GameLayer::spitSpore()
{
	int sporeCount = _player->countSpitSporeNum();

	std::string gameName = _player->getPlayerName();

	rapidjson::Document doc;                                                        //生成一个dom元素Document
	doc.SetObject();                                                                //将当前的Document设置为一个object
	rapidjson::Document::AllocatorType & allocator = doc.GetAllocator();            //获取分配器

	doc.AddMember("MsgType", MessageType::eMsg_SPIT_SPORE, allocator);
	doc.AddMember("GameName", rapidjson::Value(gameName.c_str(), allocator), allocator);
	doc.AddMember("SporeCount", sporeCount, allocator);
	/*
	json文件格式：
	{
	"MsgType":             eMsg_SPIT_SPORE,
	"GameName":            string
	"SporeCount":          int
	}
	*/

	//生成字符串
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> write(buffer);
	doc.Accept(write);

	std::string msg = buffer.GetString();

	log("__________spitSpore() send Json:\n%s", msg.c_str());

	cl.sendMsg(buffer.GetString());
}
//更新
void GameLayer::update(float dt)
{
	updateBean();               //更新豆子
	updateSpore();              //更新孢子
	updatePrick();              //更新绿刺
	_player->updateDivision();

	updateView();               //更新游戏视图
	collide();                  //碰撞检测

	synPlayerInfo();            //同步玩家信息

	synSporeInfo();             //同步孢子
}

//更新小豆豆
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

//更新孢子
void GameLayer::updateSpore()
{
	std::vector<int> vecDel;
	for (auto sporeItem : _sporeMap)
	{
		auto spore = sporeItem.second;
		if (spore != NULL)
		{
			Vec2 position = spore->getPosition();
			if (position.x<SPORE_RADIUS)
			{
				position.x = SPORE_RADIUS;
			}
			if (position.x>MAP_WIDTH - SPORE_RADIUS)
			{
				position.x = MAP_WIDTH - SPORE_RADIUS;
			}
			if (position.y<SPORE_RADIUS)
			{
				position.y = SPORE_RADIUS;
			}
			if (position.y>MAP_HEIGHT - SPORE_RADIUS)
			{
				position.y = MAP_HEIGHT - SPORE_RADIUS;
			}
			spore->setPosition(position);
			if (_player->collideSpore(spore))
			{
				vecDel.push_back(sporeItem.first);
			}
			else
			{
				for (auto item : _rivalMap)
				{
					auto rival = item.second;
					if (rival != NULL)
					{
						if (rival->collideSpore(spore))
						{
							vecDel.push_back(sporeItem.first);
							break;
						}
					}
				}
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

//以玩家为中心设置游戏地图
void GameLayer::updateView()
{
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


//执行玩家移动操作！！！！！！！！！！server
void GameLayer::playerMoveEvent(EventCustom * event)
{
	//接收到的json数据格式为：
	/*Json文件内容
	{
	"GameName":    string,
	"vx":          int,
	"vy":          int,
	}
	*/
	char * msg = (char *)event->getUserData();
	rapidjson::Document doc;
	doc.Parse<0>(msg);
	if (doc.HasParseError())
	{
		log("__________playerMoveEvent(EventCustom * even) errorno :  %d\n", doc.GetParseError());
		return;
	}

	if (doc.IsObject())
	{
		std::string gameName = doc["GameName"].GetString();
		double vx = doc["vx"].GetDouble();
		double vy = doc["vy"].GetDouble();
		auto rival = _rivalMap.at(gameName);
		if (rival != NULL)
		{
			rival->setVelocity(Vec2(vx, vy));
		}
	}
}

//向服务器发送玩家请求分身操作的信息**********client
void GameLayer::dividePlayer()
{
	_player->dividePlayer();

	std::string gameName = _player->getPlayerName();

	rapidjson::Document doc;                                                        //生成一个dom元素Document
	doc.SetObject();                                                                //将当前的Document设置为一个object
	rapidjson::Document::AllocatorType & allocator = doc.GetAllocator();            //获取分配器

																					/*
																					json文件格式：
																					{
																					"MsgType":         eMsg_DIVIDE,
																					"GameName":        gameName,
																					}
																					*/
	doc.AddMember("MsgType", MessageType::eMsg_DIVIDE, allocator);
	doc.AddMember("GameName", rapidjson::Value(gameName.c_str(), allocator), allocator);

	//生成字符串
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> write(buffer);
	doc.Accept(write);

	std::string msg = buffer.GetString();
	log("__________dividePlayer() send Json:\n%s", msg.c_str());
	cl.sendMsg(buffer.GetString());
}

//执行分身操作！！！！！！！！！！server
void GameLayer::playerDivideEvent(EventCustom * event)
{
	/*
	json文件格式：
	{
	"GameName":    string,
	}
	*/
	char * msg = (char *)event->getUserData();
	rapidjson::Document doc;
	doc.Parse<0>(msg);
	if (doc.HasParseError())
	{
		log("__________playerDivideEvent(EventCustom * event) errorno: %d\n", doc.GetParseError());
		return;
	}

	if (doc.IsObject())
	{
		std::string gameName = doc["GameName"].GetString();
		auto rival = _rivalMap.at(gameName);
		if (rival != NULL)
		{
			rival->dividePlayer();
		}
	}
}

//执行吐孢子操作！！！！！！！！！！server
void GameLayer::spitSporeResultEvent(EventCustom * event)
{
	//接收到的json数据格式为：
	/*Json文件内容
	{
	"GameName":    string,
	"GlobalID":    int,
	"SporeCount":  int,
	}
	*/
	char * msg = (char *)event->getUserData();
	rapidjson::Document doc;
	doc.Parse<0>(msg);
	if (doc.HasParseError())
	{
		log("__________spitSporeResultEvent(EventCustom * event) errorno: %d\n", doc.GetParseError());
		return;
	}

	if (doc.IsObject())
	{
		std::string gameName = _player->getPlayerName();
		std::string name = doc["GameName"].GetString();
		int globalID = doc["GlobalID"].GetInt();
		int sporeCount = doc["SporeCount"].GetInt();

		//判断是否是自己的操作
		if (gameName == name)
		{
			_player->spitSpore(_map, _sporeMap, globalID);
			for (int i = 0; i < sporeCount; i++)
			{
				_vecSporeNeedUpdate.push_back(globalID + i);
			}

		}
		else
		{
			auto rival = _rivalMap.at(name);
			rival->spitSpore(_map, _sporeMap, globalID);
		}

	}
}

//初始化绿刺
void GameLayer::initPrick(rapidjson::Value &value)
{
	//接收到的json数据格式为：
	/*Json文件内容
	{
	[ gloablID(int), x(int), y(int) ]
	}
	*/
	int size = value.Size();
	for (int i = 0; i < size; i++)
	{
		int globalID = value[i][0].GetInt();
		int x = value[i][1].GetInt();
		int y = value[i][2].GetInt();
		Prick * prick = Prick::create("GameScene/prick.png");
		prick->setPosition(x, y);
		_map->addChild(prick, prick->getScore());
		_prickMap.insert(globalID, prick);
	}
}

//更新绿刺**********client
void GameLayer::updatePrick()
{
	std::vector<int> vecDel;
	for (auto prickItem : _prickMap)
	{
		auto prick = prickItem.second;
		if (prick != NULL)
		{

			if (_player->collidePrick(prick))
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

		rapidjson::Document doc;                                                        //生成一个dom元素Document
		doc.SetObject();                                                                //将当前的Document设置为一个object
		rapidjson::Document::AllocatorType & allocator = doc.GetAllocator();            //获取分配器

																						/*
																						json文件格式：
																						{
																						"MsgType":             eMsg_EAT_PRICK,
																						"GlobalID":            int,
																						}
																						*/

		doc.AddMember("MsgType", MessageType::eMsg_EAT_PRICK, allocator);
		doc.AddMember("GlobalID", key, allocator);

		//字符串
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> write(buffer);
		doc.Accept(write);

		std::string msg = buffer.GetString();
		log("__________updatePrick() send Json:\n%s", msg.c_str());
		cl.sendMsg(buffer.GetString());
	}
	vecDel.clear();
}

//加绿刺！！！！！！！！！！server
void GameLayer::addPrickEvent(EventCustom * event)
{
	//接收到的json数据格式为：
	/*Json文件内容
	{
	"GlobalID":    int,
	"x":           int,
	"y":           int,
	}
	*/
	char * msg = (char *)event->getUserData();
	rapidjson::Document doc;
	doc.Parse<0>(msg);
	if (doc.HasParseError())
	{
		log("GetParseError %d\n", doc.GetParseError());
		return;
	}

	if (doc.IsObject())

	{
		int globalID = doc["GlobalID"].GetInt();
		int x = doc["X"].GetInt();
		int y = doc["Y"].GetInt();
		Prick * prick = Prick::create("GameScene/prick.png");
		prick->setPosition(Vec2(x, y));
		prick->setLocalZOrder(prick->getScore());
		_map->addChild(prick);
		_prickMap.insert(globalID, prick);
	}
}

//与豆子碰撞检测
void GameLayer::collideBean(Player * player)
{
	Rect rect = player->getPlayerRect();
	Vec2 position = player->getPosition();

	Vec2 point1 = Vec2(rect.origin.x - rect.size.width / 2, rect.origin.y - rect.size.height / 2);//玩家左下角
	int divisionX1 = floor(point1.x / DESIGN_SCREEN_WIDTH);
	int divisionY1 = floor(point1.y / DESIGN_SCREEN_HEIGHT);
	int division1 = divisionY1 * MAP_DIVISION_X + divisionX1;
	Vec2 point2 = Vec2(rect.origin.x + rect.size.width / 2, rect.origin.y + rect.size.height / 2);//玩家右上角
	int divisionX2 = floor(point2.x / DESIGN_SCREEN_WIDTH);
	int divisionY2 = floor(point2.y / DESIGN_SCREEN_HEIGHT);
	int division2 = divisionY2 * MAP_DIVISION_X + divisionX2;

	//处理超出界限问题
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

//碰撞检测
void GameLayer::collide()
{
	for (auto item : _rivalMap)
	{
		auto rival = item.second;
		if (rival != NULL)
		{
			if (_player->collideRival(rival))
			{
				float playerDivisionCount = _player->getDivisionNum();
				if (playerDivisionCount == 0)
				{
					_player->resetPlayer();
					break;
				}
			}
		}

	}
}


//重置小豆豆
void GameLayer::resetBean(Node * node)
{
	node->setVisible(true);
}

//重置玩家
void GameLayer::resetPlayer()
{

}

//同步玩家中和操作到服务器**********client
void GameLayer::synPlayerConcentrate()
{
	std::string gameName = _player->getPlayerName();

	rapidjson::Document doc;                                                        //生成一个dom元素Document
	doc.SetObject();                                                                //将当前的Document设置为一个object
	rapidjson::Document::AllocatorType & allocator = doc.GetAllocator();            //获取分配器

																					/*
																					json文件格式：
																					{
																					"MsgType":     eMsg_PLAYER_CONCENTRATE,
																					"GameName":    string,
																					}
																					*/
	doc.AddMember("MsgType", MessageType::eMsg_PLAYER_CONCENTRATE, allocator);
	doc.AddMember("GameName", rapidjson::Value(gameName.c_str(), allocator), allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> write(buffer);
	doc.Accept(write);

	std::string msg = buffer.GetString();

	log("__________synSporeInfo() send Json:\n%s", msg.c_str());
	cl.sendMsg(buffer.GetString());

}

//执行玩家中和操作！！！！！！！！！！server
void GameLayer::playerConcentrateEvent(EventCustom * event)
{
	//接收到的json数据格式为：
	/*Json文件内容
	{
	"GameName":        string,
	}
	*/
	char * msg = (char *)event->getUserData();
	rapidjson::Document doc;
	doc.Parse<0>(msg);
	if (doc.HasParseError())
	{
		log("GetParseError %d\n", doc.GetParseError());
		return;
	}

	if (doc.IsObject())
	{
		std::string gameName = doc["GameName"].GetString();
		auto rival = _rivalMap.at(gameName);
		if (rival != NULL)
		{
			rival->concentrate();
		}
	}
}

//更新玩家的基本信息！！！！！！！！！！server
void GameLayer::updatePlayerEvent(EventCustom * event)
{
	//接收到的json数据格式为：
	/*Json文件内容
	{
	"GameName":        string,
	"DivisionList":    { [ x(double), y(double), score(int), vx(double), vy(double) ] }
	}
	*/

	char * msg = (char *)event->getUserData();
	rapidjson::Document doc;
	doc.Parse<0>(msg);
	if (doc.HasParseError())
	{
		log("GetParseError %d\n", doc.GetParseError());
		return;
	}

	if (doc.IsObject())
	{
		std::string gameName = doc["GameName"].GetString();
		auto rival = _rivalMap.at(gameName);
		if (rival != NULL)
		{
			int oldSize = rival->getDivisionNum();
			int newSize = doc["DivisionList"].Size();
			double vx = doc["vx"].GetDouble();
			double vy = doc["vy"].GetDouble();
			rival->setVelocity(Vec2(vx, vy));

			if (oldSize != newSize)
			{
				rival->clearDivisionList();
				for (int i = 0; i < newSize; i++)
				{
					double x = doc["DivisionList"][i][0].GetDouble();
					double y = doc["DivisionList"][i][1].GetDouble();
					int score = doc["DivisionList"][i][2].GetInt();
					auto division = rival->createDivision(Vec2(x, y), Vec2(0, 0), score);
					_map->addChild(division, score);
				}

			}
			else
			{
				Vector<PlayerDivision *> divisionList = rival->getDivisionList();
				int i = 0;
				for (auto division : divisionList)
				{
					double x = doc["DivisionList"][i][0].GetDouble();
					double y = doc["DivisionList"][i][1].GetDouble();
					int score = doc["DivisionList"][i][2].GetInt();
					if (division != NULL)
					{
						division->setPosition(Vec2(x, y));
						division->setScore(score);

					}
					i++;
				}
			}

		}
	}
}


//新玩家进入事件！！！！！！！！！！server
void GameLayer::enterPlayerEvent(EventCustom * event)
{
	//接收到的json数据格式为：
	/*Json文件内容
	{
	"PlayerInfo":
	{
	"GameName":             string,
	"KeywordID":            int,
	"vx":                   double,
	"vy":                   double,
	"DivisionList:"         { [ x(double), y(double), score(int), vx(double), vy(double) ] }
	}
	}
	*/

	char * msg = (char *)event->getUserData();
	rapidjson::Document doc;
	doc.Parse<0>(msg);
	if (doc.HasParseError())
	{
		log("GetParseError %d\n", doc.GetParseError());
		return;
	}


	if (doc.IsObject())
	{
		std::string gameName = doc["PlayerInfo"]["GameName"].GetString();
		log("Player : %s enter room", gameName.c_str());

		int keywordID = doc["PlayerInfo"]["KeywordID"].GetInt();
		double vx = doc["PlayerInfo"]["vx"].GetDouble();
		double vy = doc["PlayerInfo"]["vy"].GetDouble();
		auto player = Player::create(gameName, keywordID, Vec2(vx, vy), _map);
		_map->addChild(player);
		_rivalMap.insert(gameName, player);

		int divisionNum = doc["PlayerInfo"]["DivisionList"].Size();
		for (int j = 0; j < divisionNum; j++)
		{
			double x = doc["PlayerInfo"]["DivisionList"][j][0].GetDouble();
			double y = doc["PlayerInfo"]["DivisionList"][j][1].GetDouble();
			int score = doc["PlayerInfo"]["DivisionList"][j][2].GetInt();
			double vx = doc["PlayerInfo"]["DivisionList"][j][3].GetDouble();
			double vy = doc["PlayerInfo"]["DivisionList"][j][4].GetDouble();
			auto division = player->createDivision(Vec2(x, y), Vec2(vx, vy), score);
			_map->addChild(division, score);
		}
	}
}

//更新时间
void GameLayer::timeChange(float dt)
{
	_timeCount -= dt;
}

//更新分数
void GameLayer::updateScore(float dt)
{
	int score = _player->getTotalScore();

	_eventDispatcher->dispatchCustomEvent("ScoreChange", &score);
}
/*
//更新排行榜信息
void GameLayer::updateRank(float dt)
{
Vector<Player *> vec;
for (auto item : _rivalMap)
{
vec.pushBack(item.second);
}

vec.pushBack(_player);

for (int i = 0; i < vec.size(); i++)
{
for (int j = vec.size() - 1; j > i; j--)
{
Player * player1 = vec.at(j);
Player * player2 = vec.at(j - 1);
float score1 = player1->getTotalScore();
float score2 = player2->getTotalScore();
if (score1 > score2)
{
vec.swap(j, j - 1);
}
}
}

_eventDispatcher->dispatchCustomEvent("RankChange", &vec);

}
*/

//退出
void GameLayer::onExit()
{
	this->unscheduleAllCallbacks();
	Layer::onExit();
}

/*****虚拟摇杆*****/
//虚拟遥感的操控
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
	_player->setVelocity(_joystick->getVelocity());
}

void GameLayer::onTouchEnded(Touch * touch, Event * event)
{
	_joystick->onTouchEnded(touch, event);
	_joystick->setVisible(false);
	Vec2 velocity = _joystick->getVelocity();
	if (velocity.x == 0 && velocity.y == 0)
	{
		_player->setConcentrate();
		synPlayerConcentrate();
	}
}

void GameLayer::onTouchCancelled(Touch * touch, Event * event)
{
	_joystick->onTouchCancelled(touch, event);
}