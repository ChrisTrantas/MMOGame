#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<stdio.h>
#include <iostream>
#include<winsock2.h>
//#include <sys/time.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

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
		void shutDownServer();

		static NetworkManager* networkManager;

	private:
		SOCKET s;
		struct sockaddr_in server, si_other;
		int slen, recv_len;
		char buf[BUFLEN];
		WSADATA wsa;
		bool runServer;
};

