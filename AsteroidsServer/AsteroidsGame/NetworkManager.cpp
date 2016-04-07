#include "NetworkManager.h"

NetworkManager* NetworkManager::networkManager = nullptr;

void NetworkManager::init()
{
	if (networkManager == nullptr)
	{
		networkManager = new NetworkManager();
	}
}


NetworkManager::NetworkManager()
{
	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);
	timeoutTime = 5;
}


NetworkManager::~NetworkManager()
{
	closesocket(s);
	WSACleanup();
}

int NetworkManager::displayIP()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return EXIT_FAILURE;
	}

	char ip[256];
	if (gethostname(ip, sizeof(ip)) == SOCKET_ERROR)
	{
		std::cout << "Unable to find the Host name of this computer" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "The Host name of this computer is " << ip << std::endl;

	struct hostent* phe = gethostbyname(ip);

	if (!phe)
	{
		std::cout << "Unable to find the IP address of the computer" << std::endl;
		return EXIT_FAILURE;
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i)
	{
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		std::cout << "Address " << i << ": " << inet_ntoa(addr) << std::endl;
	}
}

int NetworkManager::startServer()
{
	std::cout << "The thread started running successfully" << std::endl;
	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	timeval tv;
	fd_set fds;

	//keep listening for data
	while (runServer)
	{
		printf("Waiting for data...\n");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		std::cout << "attempting to receive data" << std::endl;

		tv.tv_sec = timeoutTime;
		tv.tv_usec = 0;

		FD_ZERO(&fds);
		FD_SET(s, &fds);

		/*if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
		{
			perror("Error");
		}*/
		int timeoutError = select(s + 1, &fds, NULL, NULL, &tv);
		if (timeoutError == -1)
		{
			printf("Error trying to timeout\n");
		}
		else if (timeoutError == 0)
		{
			printf("Client timed out\n");
		}
		else
		{
			NetworkManager::networkManager->receiveData();

			NetworkManager::networkManager->sendData();
		}
	}

	return 0;
}

int NetworkManager::sendData()
{
	//now reply the client with the same data
	if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		return EXIT_FAILURE;
	}
}

int NetworkManager::receiveData()
{
	//try to receive some data, this is a blocking call
	if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		return EXIT_FAILURE;
	}

	//print details of the client/peer and the data received
	printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	printf("Data: %s\n", buf);
}

void NetworkManager::shutDownServer()
{
	runServer = false;
}