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
#pragma comment(lib,"ws2_32.lib")  //����Socket��Ws2_32.lib��
using namespace std;



struct  ClientInfo
{
	SOCKET _ClientSock;
	sockaddr_in adClient;  //�ͻ��˵�ַ
	int ID;  //Server�����ID��������AcceptSocket������
	DWORD RecvThreadID;  //Server������յ��̺߳�
	//DWORD SendThreadID;  //Server���䷢�͵��̺߳�
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

	int CheckSocket();     //��⵱ǰ���õ�ID��

	void Process();     //����ͻ�����������

	static bool ProcessMsg(const char *msg, int ID);     //����ͻ��˷��͹�������Ϣ

	static bool SendMessageToOne(int ID, const string msg);     //��ĳһ��Client������Ϣ
	static void SendMessageToAllOther(const string msg, int ID = -1);     //������Client������Ϣ(����ID)
	static void SendMessageToAll(const string msg);     //������Client������Ϣ

	static void CleanSocket(int ID);     //���ID�ŵ��׽���(�����˳���Ϸ���߳�)

	static DWORD WINAPI GameThread(void *data); //�����̣߳������������ĸ�AcceptSocket�ĵ�ַ
	                                            //�����жϸ��߳��Ƿ�ɶ�������ɶ��Ͷ�ȡ���ϵ���Ϣ
	static DWORD WINAPI RunThread(void *data);     //ѭ����������ʱ�䲢�Ҳ��������̴�(�����¼�)

	static void AddPrick();     //���������̴��¼�
private:
	SOCKET _ServerSocket;     //�ȴ��������ݵ�socket����ʵ����Server
	sockaddr_in adServer;     //�󶨵�ַ
	                         //sockaddr_inһ���Ǵ����ַ�Ͷ˿ڵ�,������Ϣ����ʾ���洢ʹ��
	static ClientInfo AcceptSocket[MAX_NUM];  //Client�������Ϣ����ʵ����Client

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