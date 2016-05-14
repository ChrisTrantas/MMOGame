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
	OBJ_NONE,
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
	CMD_NONE,
	SERVER_UPDATE,		//CMD FROM SERVER TO CLIENT WITH EVERYONES DATA
	PLAYER_DIED,		//CMD FROM SERVER TO CLIENT THAT SAYS THE PLAYER DIED SO DISPLAY THE DEATH STUFF
	PLAYER_COMMAND,		//CMD FROM CLIENT TO SERVER THAT UPDATES THE SERVER WITH THE KEYBOARD INPUT
	//BULLET_FIRED,		//CMD FROM CLIENT TO SERVER THAT SAYS THE CLIENT IS PRESSING SPACE TO FIRE A BULLET	NO LONGER NEEDED!!!
	PLAYER_CONNECT,		//CMD FROM CLIENT TO SERVER THAT SAYS THE CLIENT IS TRYING TO CONNECT
	PLAYER_DISCONNECT	//CMD FROM CLIENT TO SERVER THAT SAYS THE CLIENT WANTS TO DISCONNECT
};

enum PlayerInput
{
	INPUT_NONE,
	LEFT,
	RIGHT,
	UP,
	FIRE,
};

struct Header
{
	int id;
	Command cmd = CMD_NONE;
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
		type = OBJ_NONE;
	}
};

struct DataUpdate
{
	int numObj;
	ObjData data;
};

struct PlayerData
{

};

struct BulletData
{
	float xPos;
	float yPos;
	float xVel;
	float yVel;
};

class NetworkManager
{
public:
	static void Init();
	NetworkManager();
	~NetworkManager();
	int DisplayIP();
	int StartServer();
	int UpdateAllClients();
	int ReceiveData();
	void UpdateData(int id, float xVel, float yVel, float rot);
	void ShutDownServer();

	//Thread Management
	void Initialize(int nThread);
	void AssignTask(void(*calback)());
	void ShutDownAllThreads();
	void ShutDownThread(DWORD dwThreadID);
	int GetFreeThread();
	std::string GetTaskMessage();
	int GetThreadCount();
	HANDLE GetMutex();

	//Server Stuff
	int GenerateID(ObjType type);
	void FreeID(int id);
	void PlayerDied(int id);
	std::vector<PlayerInput> inputBuf;

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
	//std::vector<glm::vec2> shipPos;
	//std::vector<float> shipRot;

	int SendData(sockaddr* client);
	int SendToAllClients();

	//Thread Management
	Thread* m_ptrThread[5];
	HANDLE m_hThreadPool[5];
	int m_nThreadCount;
	uint64_t ids;
};

