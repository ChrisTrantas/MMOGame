#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#include <string.h>
#include <stdint.h>
#include <vector>
#include <mutex>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
//#include <sys/time.h>

#include "Thread.h"
#include "Game.h"
#include "MathVectors.h"

//give this to Egert when I get stuck
//This is the link to my thread manager example
//http://ashishchoure.blogspot.com/2010/05/simplest-threadpool-example-using-c.html

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data
#define MAX_CLIENTS 100 //max number of clients we can have attached

enum ObjType
{
	NONE,
	PLAYER_SHIP,
	PLAYER_LIGHT,
	ASTEROID_BIG,
	ASTEROID_MEDIUM,
	ASTEROID_SMALL,
	BULLET,
	LIGHT
};

enum Command
{
	UPDATE,
	PLAYER_DIED,
	BULLET_FIRED,
	PLAYER_DISCONNECT
};

struct Header
{
	int id;
	Command cmd;
};

struct ObjData
{
	int id;
	glm::vec2 pos;
	float rot;
	ObjType type;

	ObjData()
	{
		//pos = 0;
		//rot = 0;
		type = NONE;
	}
};

struct DataUpdate
{
	int numObj;
	ObjData data;
};

struct DataFired
{
	glm::vec2 pos;
	float rot;
};

class NetworkManager
{
public:
	static void Init();
	NetworkManager();
	~NetworkManager();
	int DisplayIP();
	int StartServer();
	int SendData();
	int SendToAllClients();
	int ReceiveData();
	void UpdateData();
	void ShutDownServer();

	//Thread Management
	void Initialize(int nThread);
	void AssignTask(void(*calback)());
	void ShutDownAllThreads();
	void ShutDownThread(DWORD dwThreadID);
	int GenerateID();
	void FreeID(int id);
	int GetFreeThread();
	std::string GetTaskMessage();
	int GetThreadCount();
	HANDLE GetMutex();

	static Game* game;
	static NetworkManager* networkManager;

private:
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	WSADATA wsa;
	bool runServer;
	int timeoutTime;
	std::vector<sockaddr_in> clients;
	std::mutex bufMutex;
	Header* head;
	std::vector<ObjData> objs;
	std::vector<glm::vec2> shipPos;
	std::vector<float> shipRot;

	//Thread Management
	Thread* m_ptrThread[5];
	HANDLE m_hThreadPool[5];
	int m_nThreadCount;
	uint64_t ids;
};

