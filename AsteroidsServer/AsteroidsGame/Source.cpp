#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Game.h"
#include "NetworkManager.h"

#include <ctime>
#include <winsock2.h>
#include <thread>

#pragma comment(lib,"ws2_32.lib")

int main()
{
	Game myGame = Game();

	NetworkManager::Init();
	NetworkManager::networkManager->Initialize(4);
	NetworkManager::networkManager->DisplayIP();
	NetworkManager::game = &myGame;
	//NetworkManager::networkManager->SetGame(myGame);

	//std::thread network([](){ NetworkManager::networkManager->startServer(); });
	NetworkManager::networkManager->AssignTask([](){ NetworkManager::networkManager->StartServer(); });

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

	NetworkManager::networkManager->ShutDownServer();
	//NetworkManager::networkManager->ShutDownAllThreads();
	//network.join();
	//delete myGame;

	return 0;
}