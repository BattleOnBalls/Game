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
#define MAP_WIDTH 4800     //��ͼ��ȣ���λ����
#define MAP_HEIGHT 3600     //��ͼ�߶ȣ���λ����


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

	//Socket�������汾���
	//ͨ��WSAStartup������ɶ�Winsock����ĳ�ʼ��
	//����ϵͳ���������Socket�汾��������Ӧ��Socket�⣬Ȼ����ҵ���Socket�⵽��Ӧ�ó�����
	//�Ժ�Ӧ�ó���Ϳ��Ե����������Socket���е�����Socket������

	WSADATA wsaData;  //�����洢��WSAStartup�������ú󷵻ص�Windows Sockets����
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "��������Ϣ��Socket�汾��������\n";
		fflush(stdout);
		return;
	}

	//�����׽���
	_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  //IPv4 TCP���׽��� TCP
	if (_ServerSocket == -1)
	{
		cout << "�������׽��ֳ�ʼ�����󣬴���ţ�" << WSAGetLastError() << "\n";
		fflush(stdout);
		WSACleanup();  //�ͷ��׽�����Դ
		return;
	}

	adServer.sin_family = AF_INET;  //����Э���
	adServer.sin_port = htons(port);  //htons()�����ͱ����������ֽ�˳��ת��������ֽ�˳��
	//adServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);  //INADDR_ANY��ֵΪ0.0.0.0����ʾ�κε�ַ����
	adServer.sin_addr.s_addr = inet_addr("127.0.0.1");

	//��IP�Ͷ˿�
	int retval = bind(_ServerSocket, (LPSOCKADDR)&adServer, sizeof(adServer));
	if (retval == SOCKET_ERROR)
	{
		//if (WSAGetLastError() != 10048)
		//{
		cout << "��ʧ�ܣ�����ţ�" << WSAGetLastError() << "\n";
		fflush(stdout);
		closesocket(_ServerSocket);  //�ر��׽���
		WSACleanup();  //�ͷ��׽�����Դ
		return;
		//}
		//while (WSAGetLastError() == 10048)
		//{
			//port++;
			//adServer.sin_port = htons(port);
			//retval = bind(_ServerSocket, (LPSOCKADDR)&adServer, sizeof(adServer));
			//cout << "�󶨴��󣬴���ţ�10048" << endl;
		//}
	}
	cout << "��������IPΪ��" << adServer.sin_addr.S_un.S_addr << " �˿ں�Ϊ��" << port << "\n";
	fflush(stdout);

	//�����˿ں�IP
	//���׽�������Ϊ����ģʽ�ȴ���������(listen())
	//listen������һ��δ���ӵ��׽���ת��Ϊһ�������׽���
	//ָʾ�ں�Ӧ����ָ����׽��ֵ���������
	//����listen�����������׽��ִ�CLOSEE״̬ת����LISTEN״̬
	if (listen(_ServerSocket, 10) == SOCKET_ERROR)
	{
		cout << "������������ţ�" << WSAGetLastError() << "\n";
		fflush(stdout);
		closesocket(_ServerSocket);  //�ر��׽���
		WSACleanup();  //�ͷ��׽�����Դ
		return;
	}

	//����Ϊ������ģʽ
	unsigned long nonBlock = 1;
	if (ioctlsocket(_ServerSocket, FIONBIO, &nonBlock) == SOCKET_ERROR)
	{
		cout << "���������÷�����ģʽʧ��\n";
		fflush(stdout);
		closesocket(_ServerSocket);  //�ر��׽���
		WSACleanup();  //�ͷ��׽�����Դ
		return;
	}

	//��AcceptSocket��ʼ��
	for (int i = 0; i < MAX_NUM; i++)
		AcceptSocket[i]._ClientSock = INVALID_SOCKET;


	cout << "��������ʼ���ɹ���\n";
	fflush(stdout);
}

Server::~Server()
{
	//���������ر����ӣ���ֹ�´ζ˿ں�IP���������޷�����
	closesocket(_ServerSocket);
	cout << "�������ر�\n";
	fflush(stdout);
	//�رհ汾���
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
		if (index != -1)  //���δ��
		{
			fd_set read_set;
			FD_ZERO(&read_set);
			FD_SET(_ServerSocket, &read_set);
			select(0, &read_set, NULL, NULL, NULL);
			if (FD_ISSET(_ServerSocket, &read_set))
			{
				//�������󣬽����������󣬷���һ���µĶ�Ӧ�ڴ˴����ӵ��׽���(accept())
				//���accept�ɹ�����ô�䷵��ֵ�����ں��Զ����ɵ�һ��ȫ���׽��֣������뷵�ؿͻ���TCP����
				//�����ĵ�һ������Ϊ�����׽��֣�����ֵΪ�������׽��� 
				AcceptSocket[index]._ClientSock = accept(_ServerSocket,
					(struct sockaddr *)&AcceptSocket[index].adClient, &len);

				if (AcceptSocket[index]._ClientSock == INVALID_SOCKET)  //����ʧ��
				{
					cout << "����ʧ��\n";
					fflush(stdout);
					break;
				}

				//���÷�����
				unsigned long nonBlock = 1;
				ioctlsocket(AcceptSocket[index]._ClientSock, FIONBIO, &nonBlock);

				AcceptSocket[index].ID = index;
				//inet_ntoa()��ֵΪin�������ֽ�˳����ʽ�Ķ�����IP��ַת������"."�ָ����ַ�����ʽ
				cout << "����Ҽ��룬IP��ַΪ��" << inet_ntoa(AcceptSocket[index].adClient.sin_addr)
					<< " �˿ں�Ϊ��" << ntohs(AcceptSocket[index].adClient.sin_port) << "\n";
				fflush(stdout);


			    //�Ѹո����ӳɹ���Client����һ���µ��߳�
				HANDLE acceptThread = CreateThread(NULL, 0,
					static_cast<LPTHREAD_START_ROUTINE>(Server::GameThread),  //���߳�ִ�еĻص�������Ҳ��Ϊ�̺߳���
					(LPVOID)&AcceptSocket[index],  //�����̺߳����Ĳ���
					0, &AcceptSocket[index].RecvThreadID  //�������������ڻ���߳�ID�����ΪNULL�򲻷����߳�ID
				);
				if (acceptThread == NULL)
				{
					cout << "�����߳�ʧ��\n";
					fflush(stdout);
					ExitThread(AcceptSocket[index].RecvThreadID);  //�����߳�
				}
				cout << "Ϊ��� " << AcceptSocket[index].ID << " �����̳߳ɹ�\n";
				fflush(stdout);
			}
		}
		else  //�������
			break;
	}
}

DWORD WINAPI Server::GameThread(void *data)
{
	ClientInfo *GameSocket = static_cast<ClientInfo *>(data);
	while (true)
	{
		char recvBuf[28000];  //������

		fd_set Read;  //����selectģʽ��IO���й���
		FD_ZERO(&Read);  //�����ʼ��
		FD_SET(GameSocket->_ClientSock, &Read); //��ClientSock������У����ں˼����Щsocket�Ƿ������

		int boread = select(0, &Read, NULL, NULL, NULL);
		if (FD_ISSET(GameSocket->_ClientSock, &Read))
		{
			int get = recv(GameSocket->_ClientSock, recvBuf, sizeof(recvBuf), 0);  //������Ϣ
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
				cout << "��� " << GameSocket->ID << " ��������Ϣ��" << recvBuf << "\n";
				fflush(stdout);
				if (ProcessMsg(recvBuf, GameSocket->ID))
				{
					cout << "�˴���Ϣ��������\n";
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
					static_cast<LPTHREAD_START_ROUTINE>(Server::RunThread),  //���߳�ִ�еĻص�������Ҳ��Ϊ�̺߳���
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

	fd_set Write;  //����selectģʽ��IO���й���
	FD_ZERO(&Write);  //�����ʼ��
	FD_SET(AcceptSocket[ID]._ClientSock, &Write);  //��ClientSock�������
	select(0, NULL, &Write, NULL, NULL);  //�Ƿ��д
	if (FD_ISSET(AcceptSocket[ID]._ClientSock, &Write))  //��д
		onesend = send(AcceptSocket[ID]._ClientSock, msg.c_str(), msg.length() + sizeof(char), 0);
	else
		return false;

	if (onesend == SOCKET_ERROR)
	{
		cout << "����� " << ID << " ������Ϣʧ��\n";
		fflush(stdout);
		return false;
	}

	cout << "����� " << ID << " ��������Ϣ��" << msg << "\n";
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
		cout << "��Ϣ���ͳ��ִ���\n";
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
		cout << "��Ϣ���ͳ��ִ���\n";
	fflush(stdout);
}

void Server::CleanSocket(int ID)
{
	cout << "��� " << ID << " �˳���Ϸ\n";
	fflush(stdout);
	closesocket(AcceptSocket[ID]._ClientSock);  //���ӹر�
	AcceptSocket[ID]._ClientSock = INVALID_SOCKET;
}