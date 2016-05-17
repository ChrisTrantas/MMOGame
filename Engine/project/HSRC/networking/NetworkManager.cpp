#include "NetworkManager.h"

NetworkManager* NetworkManager::networkManager = nullptr;


void NetworkManager::init()
{
	if (networkManager == nullptr)
		networkManager = new NetworkManager();
}

void NetworkManager::Initialize(int nThread)
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	if (nThread > sysinfo.dwNumberOfProcessors)
	{
		nThread = sysinfo.dwNumberOfProcessors;
		std::cout << "That is too many threads, the system can't handle it. Reducing to " << nThread << " threads instead." << std::endl;
	}

	m_nThreadCount = nThread;

	for (int i = 0; i < nThread; i++)
	{
		m_ptrThread[i] = new Thread();
		m_ptrThread[i]->CreateWorkerThread();
		m_hThreadPool[i] = m_ptrThread[i]->GetThreadHandle();
	}
	testShip->addComponent<Ship>(new Ship());
	for (int i = 0; i < MAX_ASTEROIDS; i++)
	{
		asteroids[i] = GameObject::getGameObject();
		asteroids[i]->addComponent<Asteroid>(new Asteroid());
	}
	for (int i = 0; i < MAX_SHIPS; i++)
	{
		ships[i] = GameObject::getGameObject();
		ships[i]->addComponent<Ship>(new Ship());
	}
}

NetworkManager::NetworkManager()
{
	s, slen = sizeof(si_other);
	xPos = 0;
	yPos = 0;	

	head = (Header*)buf;


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

	printf("Enter the server IP:\n");
	gets_s(message);

	//setup address structure
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);

	long ip = inet_addr(message);

	if (ip == INADDR_NONE)
	{
		printf("That is not a correct IP. Connecting to 127.0.0.1\n");
		si_other.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	}
	else
	{
		si_other.sin_addr.S_un.S_addr = ip;
	}
	timeoutTime = 5;
	runClient = true;
}

int NetworkManager::startClient()
{
	if (networkManager == nullptr)
	{
		return EXIT_FAILURE;
	}

	recv_len = sizeof(Header);

	memset(buf, '\0', BUFLEN);
	head->id = -1;
	head->cmd = PLAYER_CONNECT;


	ObjType* type = (ObjType*)(buf + sizeof(Header));
	*type = PLAYER_SHIP;

	//send the message
	if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
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

	id = head->id;
	printf("Id of Client: %d\n", head->id);
	printf("size of int* %d\n", sizeof(int*));

	timeval tv;
	fd_set fds;
	while (runClient)
	{
		//printf("Waiting for data...\n");
		fflush(stdout);

		bufMutex.lock();
		printf("This didn't work\n");

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);
		bufMutex.unlock();

		//std::cout << "attempting to receive data" << std::endl;

		tv.tv_sec = timeoutTime;
		tv.tv_usec = 0;

		FD_ZERO(&fds);
		FD_SET(s, &fds);

		int timeoutError = select(s + 1, &fds, NULL, NULL, &tv);
		if (timeoutError == -1)
		{
			printf("Error trying to timeout\n");
		}
		else if (timeoutError == 0)
		{
			//printf("Server timed out\n");
		}
		else
		{
			NetworkManager::networkManager->receiveData();
			printf("Found some data\n");
		}
	}
	return 0;
}

int NetworkManager::sendData()
{
	head->id = id;
	printf("Sending ID: %d\n", head->id);

	//now reply the client with the same data
	if (sendto(s, buf, BUFLEN, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		return EXIT_FAILURE;
	}
	printf("Sending Data to Server \n");
}

int NetworkManager::receiveData()
{
	bufMutex.lock();
	//try to receive some data, this is a blocking call
	if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		return EXIT_FAILURE;
	}

	if (head && head->id != -1)
	{
		id = head->id;
	}

	if (head->cmd == SERVER_UPDATE)
	{
		PlayerData* data = (PlayerData*)(buf + sizeof(Header));
		//memcpy(data, buf + sizeof(Header), sizeof(PlayerData));
		printf("Got an update form the server\n");
		//testShip->getComponent<Transform>()->setPosition(vec3(data->shipPosX[0], data->shipPosY[0], 0));
		for (int i = 0; i < MAX_SHIPS; i++)
		{
			ships[i]->getComponent<Transform>()->setPosition(vec3(data->shipPosX[i], data->shipPosY[i], 0));
		}
		for (int i = 0; i < MAX_ASTEROIDS; i++)
		{
			asteroids[i]->getComponent<Transform>()->setPosition(vec3(data->astPosX[i], data->astPosY[i], 0));
		}
	}
	else if (head->cmd == PLAYER_DIED)
	{
		printf("\nThe player died! Long live the player\n\n");
		testShip->getComponent<MeshRenderer>()->enabled = false;
	}

	//print details of the client/peer and the data received
	std::cout << "Data ID: " << head->id << std::endl;
	std::cout << "Client ID: " << id << std::endl;

	bufMutex.unlock();
	
	printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	printf("Data: %s\n", buf);
	
	testShip->getComponent<Transform>()->position;
}

void NetworkManager::updateData(PlayerInput dir)
{
	bufMutex.lock();
	PlayerInput* direction = (PlayerInput*)(buf + sizeof(Header));
	*direction = dir;
	head->cmd = PLAYER_COMMAND;
	sendData();
	bufMutex.unlock();
	printf("updating Data\n");
}

void NetworkManager::serverUpdate()
{
	bufMutex.lock();
	sendData();
	bufMutex.unlock();
	printf("updating Data\n");
}

void NetworkManager::clientDisconnect()
{
	bufMutex.lock();
	head->cmd = PLAYER_DISCONNECT;
	sendData();
	bufMutex.unlock();
}

void NetworkManager::shutDownClient()
{
	runClient = false;
	clientDisconnect();
	ShutDownAllThreads();
}

void NetworkManager::ShutDownAllThreads()
{
	for (int i = 0; i < m_nThreadCount; i++)
	{
		m_ptrThread[i]->SignalShutDownEvent();
	}

	DWORD dwWaitResult = WaitForMultipleObjects(GetThreadCount(), m_hThreadPool, TRUE, INFINITE);

	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		for (int i = 0; i < m_nThreadCount; i++)
		{
			m_ptrThread[i]->ReleaseHandles();
			delete m_ptrThread[i]; // do not delete
			// use a bool array for the threads to check "am I alive"
				// if "I am not alive" have the thread return in it's method
			// set all bools to false here, and join all the threads
		}


		break;
	default:
		std::cout << "Unable to close threads " << GetLastError() << std::endl;
	}
}

int NetworkManager::GetFreeThread()
{
	for (int i = 0; i < m_nThreadCount; i++)
	{
		if (m_ptrThread[i]->IsFree())
		{
			return i;
		}
		else
		{
			std::cout << "Thread " << i << ": " << m_ptrThread[i]->GetThreadID() << " is busy." << std::endl;
		}
	}

	std::cout << "All threads are busy. Try again later." << std::endl;
	return -1;
}

void NetworkManager::AssignTask(void(*callback)())
{
	int count = GetFreeThread();
	if (count != -1)
	{
		m_ptrThread[count]->SetThreadBusy();
		m_ptrThread[count]->SetFunction(callback);
		m_ptrThread[count]->SignalWorkEvent();
	}
}

int NetworkManager::GetThreadCount()
{
	return m_nThreadCount;
}

char* const NetworkManager::GetServer()
{
	return server;
}

NetworkManager::~NetworkManager()
{
	closesocket(s);
	WSACleanup();
}
