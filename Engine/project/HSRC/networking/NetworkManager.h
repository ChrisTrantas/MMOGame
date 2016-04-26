#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<winsock2.h>
#include<stdio.h>
#include <iostream>
#include "..\threading\Thread.h"
#include "..\engine\Game.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define SERVER "127.0.0.1"  //ip address of udp server
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

struct bufferData
{
	int id = 0;
	int xPos = 0;
	int yPos = 0;
	char* data[];
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
	void updateData(WPARAM);
	void shutDownServer();


	//Thread Management
	void Initialize(int nThread);
	void AssignTask(void(*calback)());
	void ShutDownAllThreads();
	void ShutDownThread(DWORD dwThreadID);
	int GetFreeThread();
	std::string GetTaskMessage();
	int GetThreadCount();
	HANDLE GetMutex();

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
	int id = 0;

	bool runServer;
	int timeoutTime;

	//Thread Management
	Thread* m_ptrThread[5];
	HANDLE m_hThreadPool[5];
	int m_nThreadCount;
};

