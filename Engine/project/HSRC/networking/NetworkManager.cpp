#include "NetworkManager.h"

NetworkManager* NetworkManager::networkManager = nullptr;

void NetworkManager::init()
{
	if (networkManager == nullptr)
		networkManager = new NetworkManager();
}

NetworkManager::NetworkManager()
{
	s, slen = sizeof(si_other);
	xPos = 0;
	yPos = 0;


	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		//exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		//exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
}

int NetworkManager::startClient()
{
	//start communication
		printf("Enter message : ");
		gets_s(message);

		//send the message
		if (sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			//exit(EXIT_FAILURE);
		}

		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);
		//try to receive some data, this is a blocking call
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			//exit(EXIT_FAILURE);
		}

		int* xData = (int*)&buf[0];
		int* yData = (int*)&buf[8];
		printf("size of int* %d\n", sizeof(int*));
		xPos = *xData;
		yPos = *yData;
		printf("xPos: %d\n", xPos);
		printf("yPos: %d\n", yPos);
	return 0;
}

NetworkManager::~NetworkManager()
{
	closesocket(s);
	WSACleanup();
}
