#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#include <string.h>
#include <stdint.h>
//#include <sys/time.h>

#include "Thread.h"
#include "Game.h"

//give this to Egert when I get stuck
//This is the link to my thread manager example
//http://ashishchoure.blogspot.com/2010/05/simplest-threadpool-example-using-c.html

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 2048  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data
#define MAX_CLIENTS 100 //max number of clients we can have attached

struct bufferData
{
	int id = 0;
	char* data[];
};

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
	void updateData();
	void shutDownServer();

	//Thread Management
	void Initialize(int nThread);
	void AssignTask(void(*calback)());
	void ShutDownAllThreads();
	void ShutDownThread(DWORD dwThreadID);
	int GenerateID();
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

	//Thread Management
	Thread* m_ptrThread[5];
	HANDLE m_hThreadPool[5];
	int m_nThreadCount;
	uint64_t ids;
};

