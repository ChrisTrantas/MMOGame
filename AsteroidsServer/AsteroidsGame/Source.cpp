#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Game.h"

#include <ctime>
#include <iostream>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

int main()
{
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		exit(EXIT_FAILURE);
	}

	char ip[256];
	if (gethostname(ip, sizeof(ip)) == SOCKET_ERROR)
	{
		std::cout << "Unable to find the Host name of this computer" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "The Host name of this computer is " << ip << std::endl;

	struct hostent* phe = gethostbyname(ip);

	if (!phe)
	{
		std::cout << "Unable to find the IP address of the computer" << std::endl;
		exit(EXIT_FAILURE);
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i) 
	{
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		std::cout << "Address " << i << ": " << inet_ntoa(addr) << std::endl;
	}

	Game myGame = Game();

	clock_t myClock;
	myClock = clock();

	for (int i = 0; i < 20000; ++i)
	{
		myGame.Update(0.1f);
	}

	clock_t second;
	second = clock();
	float dtA = second - myClock;

	std::cout << "Average Update Time: " << (((float)dtA) / CLOCKS_PER_SEC) * 1000 / 20000.0f << "ms" << std::endl;

	int b = 0;
	std::cin >> b;

	return 0;
}