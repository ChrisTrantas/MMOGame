#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<winsock2.h>
#include<stdio.h>
#include <iostream>
#include <mutex>
#include "..\threading\Thread.h"
#include "..\engine\Game.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

//#define SERVER "127.0.0.1"  //ip address of udp server
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

struct bufferData
{
	int id;
	int xPos;
	int yPos;
	bool alive;
	bool fired;
	bool connected;
	//char* data[];
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
	void updateData(char);
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
	bufferData* data;

	std::mutex bufMutex;

	//Thread Management
	Thread* m_ptrThread[5];
	HANDLE m_hThreadPool[5];
	int m_nThreadCount;
};

