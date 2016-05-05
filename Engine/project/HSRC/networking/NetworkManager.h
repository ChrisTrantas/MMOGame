#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<winsock2.h>
#include<stdio.h>
#include <iostream>
#include <mutex>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include "..\threading\Thread.h"
#include "..\engine\Game.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

//#define SERVER "127.0.0.1"  //ip address of udp server
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

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
	glm::vec2* pos;
	float* rot;
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
	ObjData* data;
};

struct DataFired
{
	glm::vec2* pos;
	float rot;
};

class NetworkManager
{
public:
	static void init();
	static Game* game;
	

	NetworkManager();
	~NetworkManager();

	int displayIP();
	int startServer();
	int sendData();
	int receiveData();
	void updateData(glm::vec3 pos, glm::vec3 rot);
	void serverUpdate();
	void died();
	void fired();
	void clientDisconnect();
	void shutDownClient();


	//Thread Management
	void Initialize(int nThread);
	void AssignTask(void(*calback)());
	void ShutDownAllThreads();
	void ShutDownThread(DWORD dwThreadID);
	int GetFreeThread();
	std::string GetTaskMessage();
	int GetThreadCount();
	HANDLE GetMutex();
	char* const GetServer();

	static NetworkManager* networkManager;
	static Thread* threadManager;
	int startClient();

private:
	struct sockaddr_in si_other;
	int s, slen, recv_len;;
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;
	int xPos;
	int yPos;
	int id;

	bool runClient;
	int timeoutTime;
	char* server;
	Header* head;

	std::mutex bufMutex;

	//Thread Management
	Thread* m_ptrThread[5];
	HANDLE m_hThreadPool[5];
	int m_nThreadCount;
};

