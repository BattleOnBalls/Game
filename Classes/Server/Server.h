#ifndef _SERVER_H_
#define _SERVER_H_

#define MAX_NUM 4

#include <WinSock2.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <inttypes.h>
//#include <new>
//#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#pragma comment(lib,"ws2_32.lib")  //链接Socket的Ws2_32.lib库
using namespace std;



struct  ClientInfo
{
	SOCKET _ClientSock;
	sockaddr_in adClient;  //客户端地址
	int ID;  //Server分配的ID，就是在AcceptSocket里的序号
	DWORD RecvThreadID;  //Server分配接收的线程号
	//DWORD SendThreadID;  //Server分配发送的线程号
};


struct Prick
{
	int x;
	int y;
};

struct Spore
{
	int sporeid;
	double x;
	double y;
};

struct PlayerDivision
{
	double x;
	double y;
	int score;
	double vx;
	double vy;
};

struct Player
{
	string name;
	int skinid;
	double vx;
	double vy;
	vector<PlayerDivision> division;
};





class Server
{
public:
	Server();
	~Server();

	void InitData();

	int CheckSocket();     //检测当前可用的ID号

	void Process();     //处理客户端连接请求

	static bool ProcessMsg(const char *msg, int ID);     //处理客户端发送过来的信息

	static bool SendMessageToOne(int ID, const string msg);     //向某一个Client发送信息
	static void SendMessageToAllOther(const string msg, int ID = -1);     //向所有Client发送信息(除了ID)
	static void SendMessageToAll(const string msg);     //向所有Client发送信息

	static void CleanSocket(int ID);     //清空ID号的套接字(清理退出游戏的线程)

	static DWORD WINAPI GameThread(void *data); //接受线程，传进来具体哪个AcceptSocket的地址
	                                            //首先判断该线程是否可读，如果可读就读取其上的信息
	static DWORD WINAPI RunThread(void *data);     //循环处理，更新时间并且不断增加绿刺(触发事件)

	static void AddPrick();     //触发增加绿刺事件
private:
	SOCKET _ServerSocket;     //等待接受数据的socket，其实就是Server
	sockaddr_in adServer;     //绑定地址
	                         //sockaddr_in一般是储存地址和端口的,用于信息的显示及存储使用
	static ClientInfo AcceptSocket[MAX_NUM];  //Client的相关信息，其实就是Client

	static map<int, Prick> pricklist;
	static map<int, Spore> sporelist;
	static map<string, Player> playerlist;

	static bool flag;
	static double passtime;
	static double starttime;
	static int seed;
	static int addPrickTime;
	static int globalID;
};

#endif