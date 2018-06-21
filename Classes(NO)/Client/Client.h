#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <WinSock2.h>
#include <stdio.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")  //Á´½ÓSocketµÄWs2_32.lib¿â
#include "cocos2d.h"
USING_NS_CC;
using namespace std;

class Client
{
public:
	Client();
	~Client();

	void sendMsg(const string msg);

	static DWORD WINAPI RecvThread(LPVOID lpParam);
	static void processMsg(const char* msg);

	void setSkin(const int skin);
	int getSkin();
	void getConfig(string &str);
private:
	SOCKET _ClientSocket;
	sockaddr_in adServer;

	int _myskin;
	static string _config;
};

#endif