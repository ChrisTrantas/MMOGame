#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Game.h"
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

	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		std::cout << "Address " << i << ": " << inet_ntoa(addr) << std::endl;
	}

	Game myGame = Game();
	for (int i = 0; i < 1000; ++i){
		myGame.Update(1.0f);
	}
	return 0;
}