#include "Client.h"
#include <iostream>
#include "Header/MessageType.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/reader.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
using namespace rapidjson;

#define port 8080

string Client::_config;


Client::Client()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		log("WSAStartup wrong\n");
		return;
	}

	_ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_ClientSocket == INVALID_SOCKET)
	{
		log("socket init wrong, errorno: %d\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	adServer.sin_family = AF_INET;
	adServer.sin_port = htons(port);
	adServer.sin_addr.s_addr = inet_addr("127.0.0.1");

	unsigned long ul = 1;
	int ret = ioctlsocket(_ClientSocket, FIONBIO, (unsigned long*)&ul);  //设成非阻塞模式
	if (ret == SOCKET_ERROR)
	{
		log("ioctlsocket wrong\n");
		closesocket(_ClientSocket);
		WSACleanup();
		return;
	}

	if (connect(_ClientSocket, (LPSOCKADDR)&adServer, sizeof(adServer)) == SOCKET_ERROR)
	{
		struct timeval tm;
		fd_set wfd;
		FD_ZERO(&wfd);
		FD_SET(_ClientSocket, &wfd);
		tm.tv_sec = 0.1;
		tm.tv_usec = 0;
		int sel = select(0, NULL, &wfd, NULL, &tm);
		if (sel <= 0)
		{
			log("Not connect Server\n");
			closesocket(_ClientSocket);
			WSACleanup();
			return;
		}
	}
	log("Connect Server Succeed!\n");

	HANDLE hThread;
	hThread = CreateThread(NULL, 0,
		static_cast<LPTHREAD_START_ROUTINE>(Client::RecvThread),
		static_cast<LPVOID>(this),
		0, NULL);
	CloseHandle(hThread);

	_myskin = 0;
}

Client::~Client()
{
	closesocket(_ClientSocket);
	WSACleanup();
}

DWORD WINAPI Client::RecvThread(LPVOID lpParam)
{
	Client *CClient = static_cast<Client *>(lpParam);
	while (true)
	{
		char recvBuf[28000];
		fd_set rfd;
		FD_ZERO(&rfd);
		FD_SET(CClient->_ClientSocket, &rfd);
		int boread = select(0, &rfd, NULL, NULL, NULL);
		if (FD_ISSET(CClient->_ClientSocket, &rfd))
		{
			int get = recv(CClient->_ClientSocket, recvBuf, sizeof(recvBuf), 0);
			if (get == SOCKET_ERROR)
			{
				closesocket(CClient->_ClientSocket);
				break;
			}
			if (get > 0 && get < 28000)
			{
				recvBuf[get] = '\0';
				log("Server send: %s\n", recvBuf);
				processMsg(recvBuf);
			}
		}
	}

	return 1;
}

void Client::processMsg(const char* msg)
{
	Document doc;
	doc.Parse<0>(msg);

	if (doc.HasParseError())  //打印解析错误
	{
		log("GetParseError %d\n", doc.GetParseError());
		return;
	}

	if (doc.IsObject() && doc.HasMember("MsgType"))
	{
		int msgType = doc["MsgType"].GetInt();

		if (msgType == MessageType::eMsg_START_GAME_SINGLE_RESULT)
		{
			_config = msg;
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("GameStart");
		}
		else if (msgType == MessageType::eMsg_ADD_PRICK)
		{
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("AddPrick", (void *)msg);
		}
		else if (msgType == MessageType::eMsg_UPDATE_TIME)
		{
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("UpdateTime", (void *)msg);
		}
		else if (msgType == MessageType::eMsg_ENTER_PLAYER)
		{
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("EnterPlayer", (void *)msg);
		}
		else if (msgType == MessageType::eMsg_MOVE)
		{
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("PlayerMove", (void *)msg);
		}
		else if (msgType == MessageType::eMsg_DIVIDE)
		{
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("PlayerDivide", (void *)msg);
		}
		else if (msgType == MessageType::eMsg_UPDATE_POSITION)
		{
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("UpdatePlayer", (void *)msg);
		}
		else if (msgType == MessageType::eMsg_SPIT_SPORE_RESULT)
		{
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("SpitSporeResult", (void *)msg);
		}
		else if (msgType == MessageType::eMsg_PLAYER_CONCENTRATE)
		{
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("PlayerConcentrate", (void *)msg);
		}
	}
}

void Client::sendMsg(const string msg)
{
	fd_set wfd;
	FD_ZERO(&wfd);
	FD_SET(_ClientSocket, &wfd);
	select(0, NULL, &wfd, NULL, NULL);
	if (FD_ISSET(_ClientSocket, &wfd))
		send(_ClientSocket, msg.c_str(), msg.length(), 0);
}

void Client::getConfig(string &str)
{
	str = _config;
}

/*
void Client::setName(const string myname)
{
	_myname = myname;
}

string Client::getName()
{
	return _myname;
}
*/

void Client::setSkin(const int skin)
{
	_myskin = skin;
}

int Client::getSkin()
{
	return _myskin;
}