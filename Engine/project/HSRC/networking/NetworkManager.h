#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<winsock2.h>
#include<stdio.h>
#include <iostream>
#include "..\threading\Thread.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define SERVER "127.0.0.1"  //ip address of udp server
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

class NetworkManager
{
public:
	static void init();
	NetworkManager();
	~NetworkManager();

	int displayIP();
	int startServer();
	int sendData();
	int receiveData();
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
	int startClient();
private:
	struct sockaddr_in si_other;
	int s, slen, recv_len;;
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;
	int xPos;
	int yPos;

	bool runServer;
	int timeoutTime;

	//Thread Management
	Thread* m_ptrThread[5];
	HANDLE m_hThreadPool[5];
	int m_nThreadCount;
};

