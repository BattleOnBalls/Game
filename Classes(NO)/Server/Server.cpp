#include "Server.h"
#include "MessageType.h"
#include <iostream>
#include <time.h>
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/reader.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
using namespace rapidjson;

#define port 8080
#define MAP_WIDTH 4800     //地图宽度，单位像素
#define MAP_HEIGHT 3600     //地图高度，单位像素


ClientInfo Server::AcceptSocket[MAX_NUM];

map<int, Prick> Server::pricklist;
map<int, Spore> Server::sporelist;
map<string, Player> Server::playerlist;

bool Server::flag = true;
double Server::starttime = 0;
double Server::passtime = 0;
int Server::seed = rand() % 100 + 1;
int Server::addPrickTime = 10;
int Server::globalID = 0;


Server::Server()
{
	_ServerSocket = -1;

	//Socket服务器版本监测
	//通过WSAStartup函数完成对Winsock服务的初始化
	//操作系统根据请求的Socket版本来搜索相应的Socket库，然后绑定找到的Socket库到该应用程序中
	//以后应用程序就可以调用所请求的Socket库中的其它Socket函数了

	WSADATA wsaData;  //用来存储被WSAStartup函数调用后返回的Windows Sockets数据
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "服务器信息：Socket版本出现问题\n";
		fflush(stdout);
		return;
	}

	//创建套接字
	_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  //IPv4 TCP流套接字 TCP
	if (_ServerSocket == -1)
	{
		cout << "服务器套接字初始化错误，错误号：" << WSAGetLastError() << "\n";
		fflush(stdout);
		WSACleanup();  //释放套接字资源
		return;
	}

	adServer.sin_family = AF_INET;  //设置协议簇
	adServer.sin_port = htons(port);  //htons()将整型变量从主机字节顺序转变成网络字节顺序
	//adServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);  //INADDR_ANY数值为0.0.0.0，表示任何地址都行
	adServer.sin_addr.s_addr = inet_addr("127.0.0.1");

	//绑定IP和端口
	int retval = bind(_ServerSocket, (LPSOCKADDR)&adServer, sizeof(adServer));
	if (retval == SOCKET_ERROR)
	{
		//if (WSAGetLastError() != 10048)
		//{
		cout << "绑定失败，错误号：" << WSAGetLastError() << "\n";
		fflush(stdout);
		closesocket(_ServerSocket);  //关闭套接字
		WSACleanup();  //释放套接字资源
		return;
		//}
		//while (WSAGetLastError() == 10048)
		//{
			//port++;
			//adServer.sin_port = htons(port);
			//retval = bind(_ServerSocket, (LPSOCKADDR)&adServer, sizeof(adServer));
			//cout << "绑定错误，错误号：10048" << endl;
		//}
	}
	cout << "服务器的IP为：" << adServer.sin_addr.S_un.S_addr << " 端口号为：" << port << "\n";
	fflush(stdout);

	//监听端口和IP
	//将套接字设置为监听模式等待连接请求(listen())
	//listen函数把一个未连接的套接字转换为一个被动套接字
	//指示内核应接受指向该套接字的连接请求
	//调用listen函数将导致套接字从CLOSEE状态转换到LISTEN状态
	if (listen(_ServerSocket, 10) == SOCKET_ERROR)
	{
		cout << "监听出错，错误号：" << WSAGetLastError() << "\n";
		fflush(stdout);
		closesocket(_ServerSocket);  //关闭套接字
		WSACleanup();  //释放套接字资源
		return;
	}

	//设置为非阻塞模式
	unsigned long nonBlock = 1;
	if (ioctlsocket(_ServerSocket, FIONBIO, &nonBlock) == SOCKET_ERROR)
	{
		cout << "服务器设置非阻塞模式失败\n";
		fflush(stdout);
		closesocket(_ServerSocket);  //关闭套接字
		WSACleanup();  //释放套接字资源
		return;
	}

	//将AcceptSocket初始化
	for (int i = 0; i < MAX_NUM; i++)
		AcceptSocket[i]._ClientSock = INVALID_SOCKET;


	cout << "服务器初始化成功！\n";
	fflush(stdout);
}

Server::~Server()
{
	//析构函数关闭连接，防止下次端口和IP被监听，无法开启
	closesocket(_ServerSocket);
	cout << "服务器关闭\n";
	fflush(stdout);
	//关闭版本监测
	WSACleanup();
}

void Server::InitData()
{
	
}

int Server::CheckSocket()
{
	for (int i = 0; i < MAX_NUM; i++)
	{
		if (AcceptSocket[i]._ClientSock == INVALID_SOCKET)
			return i;
	}
	return -1;
}


void Server::Process()
{
	while (true)
	{
		int index = CheckSocket();
		int len = sizeof(SOCKADDR);
		if (index != -1)  //玩家未满
		{
			fd_set read_set;
			FD_ZERO(&read_set);
			FD_SET(_ServerSocket, &read_set);
			select(0, &read_set, NULL, NULL, NULL);
			if (FD_ISSET(_ServerSocket, &read_set))
			{
				//请求到来后，接受连接请求，返回一个新的对应于此次连接的套接字(accept())
				//如果accept成功，那么其返回值是由内核自动生成的一个全新套接字，代表与返回客户的TCP连接
				//函数的第一个参数为监听套接字，返回值为已连接套接字 
				AcceptSocket[index]._ClientSock = accept(_ServerSocket,
					(struct sockaddr *)&AcceptSocket[index].adClient, &len);

				if (AcceptSocket[index]._ClientSock == INVALID_SOCKET)  //连接失败
				{
					cout << "连接失败\n";
					fflush(stdout);
					break;
				}

				//设置非阻塞
				unsigned long nonBlock = 1;
				ioctlsocket(AcceptSocket[index]._ClientSock, FIONBIO, &nonBlock);

				AcceptSocket[index].ID = index;
				//inet_ntoa()将值为in的网络字节顺序形式的二进制IP地址转换成以"."分隔的字符串形式
				cout << "新玩家加入，IP地址为：" << inet_ntoa(AcceptSocket[index].adClient.sin_addr)
					<< " 端口号为：" << ntohs(AcceptSocket[index].adClient.sin_port) << "\n";
				fflush(stdout);


			    //把刚刚连接成功的Client建立一个新的线程
				HANDLE acceptThread = CreateThread(NULL, 0,
					static_cast<LPTHREAD_START_ROUTINE>(Server::GameThread),  //被线程执行的回调函数，也称为线程函数
					(LPVOID)&AcceptSocket[index],  //传入线程函数的参数
					0, &AcceptSocket[index].RecvThreadID  //传出参数，用于获得线程ID，如果为NULL则不返回线程ID
				);
				if (acceptThread == NULL)
				{
					cout << "创建线程失败\n";
					fflush(stdout);
					ExitThread(AcceptSocket[index].RecvThreadID);  //结束线程
				}
				cout << "为玩家 " << AcceptSocket[index].ID << " 创建线程成功\n";
				fflush(stdout);
			}
		}
		else  //玩家已满
			break;
	}
}

DWORD WINAPI Server::GameThread(void *data)
{
	ClientInfo *GameSocket = static_cast<ClientInfo *>(data);
	while (true)
	{
		char recvBuf[28000];  //缓冲区

		fd_set Read;  //基于select模式对IO进行管理
		FD_ZERO(&Read);  //清零初始化
		FD_SET(GameSocket->_ClientSock, &Read); //将ClientSock加入队列，让内核检测这些socket是否读就绪

		int boread = select(0, &Read, NULL, NULL, NULL);
		if (FD_ISSET(GameSocket->_ClientSock, &Read))
		{
			int get = recv(GameSocket->_ClientSock, recvBuf, sizeof(recvBuf), 0);  //接收信息
			if (get == SOCKET_ERROR)
			{
				CleanSocket(GameSocket->ID);
				return 1;
			}
			if (boread == 1 && get == 0 && errno != EINTR)
			{
				CleanSocket(GameSocket->ID);
				return 1;
			}
			if (get > 0 && get < 28000)
			{
				recvBuf[get] = '\0';
				cout << "玩家 " << GameSocket->ID << " 发送了消息：" << recvBuf << "\n";
				fflush(stdout);
				if (ProcessMsg(recvBuf, GameSocket->ID))
				{
					cout << "此次信息传递有误\n";
					fflush(stdout);
					continue;
				}
			}
		}
	}
}


bool Server::ProcessMsg(const char *msg, int ID)
{
	Document doc;
	doc.Parse<0>(msg);

	if (doc.HasParseError())
		return false;

	if (doc.IsObject() && doc.HasMember("MsgType"))
	{
		int msgType = doc["MsgType"].GetInt();

		if (msgType == MessageType::eMsg_START_GAME_SINGLE)
		{
			if (flag)
			{
				HANDLE acceptThread = CreateThread(NULL, 0,
					static_cast<LPTHREAD_START_ROUTINE>(Server::RunThread),  //被线程执行的回调函数，也称为线程函数
					NULL, 0, NULL);
				flag = false;
				starttime = time(nullptr);
			}
			else
				passtime = time(nullptr) - starttime;


			Document result;
			Document::AllocatorType &ar = result.GetAllocator();

			Value type;
			type.SetInt(MessageType::eMsg_START_GAME_SINGLE_RESULT);
			result.AddMember("MsgType", type, ar);  //MsgType
			result.AddMember("RoomSeed", seed, ar);  //RoomSeed
			result.AddMember("PassTime", passtime, ar);  //PassTime

			//
			//PrickList
			//
			Document prickall;
			prickall.SetArray();
			Document::AllocatorType &ap = prickall.GetAllocator();
			for (const auto &v : pricklist)
			{
				Document temp;
				temp.SetArray();
				Document::AllocatorType &at = temp.GetAllocator();
				temp.PushBack(v.first, at);
				temp.PushBack(v.second.x, at);
				temp.PushBack(v.second.y, at);
				prickall.PushBack(temp, ap);
			}
			result.AddMember("PrickList", prickall, ar);

			//
			//SporeList
			//
			Document sporeall;
			sporeall.SetArray();
			Document::AllocatorType &as = sporeall.GetAllocator();
			for (const auto &v : sporelist)
			{
				Document temp;
				temp.SetArray();
				Document::AllocatorType &at = temp.GetAllocator();
				temp.PushBack(v.first, at);
				temp.PushBack(v.second.sporeid, at);
				temp.PushBack(v.second.x, at);
				temp.PushBack(v.second.y, at);
				sporeall.PushBack(temp, as);
			}
			result.AddMember("SporeList", sporeall, ar);

			//
			//Player
			//
			Player thisone;
			PlayerDivision one;
			if (doc.HasMember("GameName"))
				thisone.name = doc["GameName"].GetString();
			if (doc.HasMember("KeywordID"))
				thisone.skinid = doc["KeywordID"].GetInt();
			thisone.vx = 0, thisone.vy = 0;
			one.vx = 0, one.vy = 0;
			one.x = rand() % (MAP_WIDTH - 20) + 20;
			one.y = rand() % (MAP_HEIGHT - 20) + 20;
			one.score = 10;
			thisone.division.push_back(one);
			playerlist[thisone.name] = thisone;

			Document play;
			play.SetObject();
			Document::AllocatorType &aplay = play.GetAllocator();
			play.AddMember("GameName", Value(thisone.name.c_str(), aplay), aplay);
			play.AddMember("KeywordID", thisone.skinid, aplay);
			play.AddMember("vx", thisone.vx, aplay);
			play.AddMember("vy", thisone.vy, aplay);
			Document division;
			division.SetArray();
			Document::AllocatorType &adi = division.GetAllocator();
			for (const auto &v : thisone.division)
			{
				Document temp;
				temp.SetArray();
				Document::AllocatorType &at = temp.GetAllocator();
				temp.PushBack(v.x, at);
				temp.PushBack(v.y, at);
				temp.PushBack(v.score, at);
				temp.PushBack(v.vx, at);
				temp.PushBack(v.vy, at);
				division.PushBack(temp, adi);
			}
			play.AddMember("DivisionList", division, aplay);
			result.AddMember("Player", play, ar);

			Document enterplay;
			enterplay.SetObject();
			Document::AllocatorType &ae = enterplay.GetAllocator();
			Value ttype;
			ttype.SetInt(MessageType::eMsg_ENTER_PLAYER);
			enterplay.AddMember("MsgType", ttype, ae);
			enterplay.AddMember("PlayerInfo", play, ae);
			StringBuffer buffer1;
			Writer<StringBuffer>write1(buffer1);
			enterplay.Accept(write1);
			SendMessageToAllOther(buffer1.GetString(), ID);

			//
			//RivalList
			//
			Document rival;
			rival.SetArray();
			Document::AllocatorType &ari = rival.GetAllocator();
			for (const auto &i : playerlist)
			{
				if (i.first != thisone.name)
				{
					Document onerival;
					onerival.SetObject();
					Document::AllocatorType &aor = onerival.GetAllocator();
					onerival.AddMember("GameName", Value(i.second.name.c_str(), aor), aor);
					onerival.AddMember("KeywordID", i.second.skinid, aor);
					onerival.AddMember("vx", i.second.vx, aor);
					onerival.AddMember("vy", i.second.vy, aor);
					Document div;
					div.SetArray();
					Document::AllocatorType &adiv = div.GetAllocator();
					for (const auto &j : i.second.division)
					{
						Document temp;
						temp.SetArray();
						Document::AllocatorType &at = temp.GetAllocator();
						temp.PushBack(j.x, at);
						temp.PushBack(j.y, at);
						temp.PushBack(j.score, at);
						temp.PushBack(j.vx, at);
						temp.PushBack(j.vy, at);
						div.PushBack(temp, adiv);
					}
					onerival.AddMember("DivisionList", div, aor);
					rival.PushBack(onerival, ari);
				}
			}
			result.AddMember("RivalList", rival, ar);

			StringBuffer buffer2;
			Writer<StringBuffer>writer2(buffer2);
			result.Accept(writer2);
			SendMessageToOne(ID, buffer2.GetString());
		}

		else if (msgType == MessageType::eMsg_MOVE)
		{
			SendMessageToAllOther(msg, ID);
		}

		else if (msgType == MessageType::eMsg_DIVIDE)
		{
			SendMessageToAllOther(msg, ID);
		}

		else if (msgType == MessageType::eMsg_UPDATE_POSITION)
		{
			int i = doc["DivisionList"].Size();
			vector<PlayerDivision>().swap(playerlist[doc["GameName"].GetString()].division);
			for (int j = 0; j < i; j++)
			{
				PlayerDivision temp;
				temp.x = doc["DivisionList"][i][0].GetDouble();
				temp.y = doc["DivisionList"][i][1].GetDouble();
				temp.score = doc["DivisionList"][i][2].GetInt();
				temp.vx = playerlist[doc["GameName"].GetString()].vx;
				temp.vy = playerlist[doc["GameName"].GetString()].vy;
				playerlist[doc["GameName"].GetString()].division.push_back(temp);
			}
			SendMessageToAllOther(msg, ID);
		}

		else if (msgType == MessageType::eMsg_SPIT_SPORE)
		{
			Document result;
			result.SetObject();
			Document::AllocatorType &allocator = result.GetAllocator();
			globalID += doc["SporeCount"].GetInt();
			Value type;
			type.SetInt(MessageType::eMsg_SPIT_SPORE_RESULT);
			result.AddMember("MsgType", type, allocator);
			result.AddMember("GameName", Value(doc["GameName"].GetString(), allocator), allocator);
			result.AddMember("SporeCount", doc["SporeCount"].GetInt(), allocator);
			StringBuffer buffer;
			Writer<StringBuffer>writer(buffer);
			result.Accept(writer);
			SendMessageToAll(buffer.GetString());
		}

		else if (msgType == MessageType::eMsg_PLAYER_CONCENTRATE)
		{
			SendMessageToAllOther(msg, ID);
		}

		else if (msgType == MessageType::eMsg_UPDATE_SPORE)
		{
			int i = doc["SporeList"].Size();
			for (int j = 0; j < i; j++)
			{
				map<int, Spore>::iterator it;
				it = sporelist.find(doc["SporeList"][i][0].GetInt());
				if (it != sporelist.end())
				{
					(*it).second.x = doc["SporeList"][i][2].GetDouble();
					(*it).second.y = doc["SporeList"][i][3].GetDouble();
				}
				else
					sporelist[doc["SporeList"][i][0].GetInt()] = { doc["SporeList"][i][1].GetInt(),
						doc["SporeList"][i][2].GetDouble() ,doc["SporeList"][i][3].GetDouble() };

			}
		}

		else if (msgType == MessageType::eMsg_EAT_PRICK)
		{
			pricklist.erase(doc["GlobalID"].GetInt());
		}

		return true;
	}
	return false;
}

DWORD WINAPI Server::RunThread(void *data)
{
	while (true)
	{
		double now = time(nullptr);
		passtime = now - starttime;
		if (passtime >= addPrickTime)
		{
			addPrickTime += 10;
			AddPrick();
		}

		Document result;
		result.SetObject();
		Document::AllocatorType &allocator = result.GetAllocator();
		Value type;
		type.SetInt(MessageType::eMsg_UPDATE_TIME);
		result.AddMember("MsgType", type, allocator);
		result.AddMember("TimeCount", passtime, allocator);

		StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> write(buffer);
		result.Accept(write);
		const string msg = buffer.GetString();
		SendMessageToAll(msg);

		if (passtime >= 100)
			break;
		Sleep(1000);
	}
	return 1;
}

void Server::AddPrick()
{
	int x = rand() % (MAP_WIDTH - 250) + 125;
	int y = rand() % (MAP_HEIGHT - 250) + 125;
	pricklist[globalID] = { x,y };

	Document result;
	result.SetObject();
	Document::AllocatorType &allocator = result.GetAllocator();
	Value type;
	type.SetInt(MessageType::eMsg_ADD_PRICK);
	result.AddMember("MsgType", type, allocator);
	result.AddMember("GlobalID", globalID, allocator);
	result.AddMember("X", x, allocator);
	result.AddMember("Y", y, allocator);

	StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> write(buffer);
	result.Accept(write);
	const string msg = buffer.GetString();
	SendMessageToAll(msg);
}

bool Server::SendMessageToOne(int ID, const string  msg)
{
	int onesend;

	fd_set Write;  //基于select模式对IO进行管理
	FD_ZERO(&Write);  //清零初始化
	FD_SET(AcceptSocket[ID]._ClientSock, &Write);  //将ClientSock加入队列
	select(0, NULL, &Write, NULL, NULL);  //是否可写
	if (FD_ISSET(AcceptSocket[ID]._ClientSock, &Write))  //可写
		onesend = send(AcceptSocket[ID]._ClientSock, msg.c_str(), msg.length() + sizeof(char), 0);
	else
		return false;

	if (onesend == SOCKET_ERROR)
	{
		cout << "向玩家 " << ID << " 发送消息失败\n";
		fflush(stdout);
		return false;
	}

	cout << "向玩家 " << ID << " 发送了消息：" << msg << "\n";
	fflush(stdout);
	return true;
}

void Server::SendMessageToAllOther(const string  msg, int ID)
{
	bool bo = true;
	for (int i = 0; i < MAX_NUM; i++)
	{
		if (ID != i)
		{
			if (AcceptSocket[i]._ClientSock != INVALID_SOCKET && !SendMessageToOne(i, msg))
				bo = false;
		}
	}

	if (!bo)
		cout << "信息发送出现错误\n";
	fflush(stdout);
}

void Server::SendMessageToAll(const string msg)
{
	bool bo = true;
	for (int i = 0; i < MAX_NUM; i++)
	{
		if (AcceptSocket[i]._ClientSock != INVALID_SOCKET)
		{
			if (!SendMessageToOne(i, msg))
				bo = false;
		}
	}
	if (!bo)
		cout << "信息发送出现错误\n";
	fflush(stdout);
}

void Server::CleanSocket(int ID)
{
	cout << "玩家 " << ID << " 退出游戏\n";
	fflush(stdout);
	closesocket(AcceptSocket[ID]._ClientSock);  //连接关闭
	AcceptSocket[ID]._ClientSock = INVALID_SOCKET;
}