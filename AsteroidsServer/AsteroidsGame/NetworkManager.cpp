#include "NetworkManager.h"

NetworkManager* NetworkManager::networkManager = nullptr;
Game* NetworkManager::game = nullptr;

void NetworkManager::Init()
{
	if (networkManager == nullptr)
	{
		networkManager = new NetworkManager();
	}
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
}

NetworkManager::NetworkManager()
{
	slen = sizeof(si_other);
	head = (Header*)buf;

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
	runServer = true;
	//game = NULL;
}


NetworkManager::~NetworkManager()
{
	closesocket(s);
	WSACleanup();
}

int NetworkManager::DisplayIP()
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

	return EXIT_SUCCESS;
}

int NetworkManager::StartServer()
{
	if (networkManager == nullptr)
	{
		return EXIT_FAILURE;
	}

	ids = 0;

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

		bufMutex.lock();
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);
		bufMutex.unlock();

		std::cout << "attempting to receive data" << std::endl;

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
			printf("Client timed out\n");
		}
		else
		{
			NetworkManager::networkManager->ReceiveData();
			//NetworkManager::networkManager->UpdateData();
			//NetworkManager::networkManager->SendData();
		}

		//NetworkManager::networkManager->SendToAllClients();
	}

	delete networkManager;

	return 0;
}

int NetworkManager::SendData(sockaddr* client)
{
	std::cout << "Sending ID " << head->id << std::endl;

	//now reply the client with the same data
	if (sendto(s, buf, BUFLEN, 0, client, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		return EXIT_FAILURE;
	}

	printf("Data sent to client \n");
	printf("Id sent to client: %d\n", ids);
	return EXIT_SUCCESS;
}

int NetworkManager::SendToAllClients()
{
	for (int i = 0; i < clients.size(); i++)
	{
		SendData((struct sockaddr*) &clients[i]);
	}

	return EXIT_SUCCESS;
}

int NetworkManager::ReceiveData()
{
	bufMutex.lock();
	//try to receive some data, this is a blocking call
	if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d", WSAGetLastError());
		return EXIT_FAILURE;
	}

	//print details of the client/peer and the data received
	printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	printf("Data: %s\n", buf);
	std::cout << "ID on receive: " << head->id << std::endl;

	if (head && head->cmd == PLAYER_DISCONNECT)
	{
		FreeID(head->id);
		game->SetShipAlive(head->id, false);
	}
	else if (head && head->cmd == PLAYER_CONNECT)
	{
		head->id = GenerateID(*((ObjType*)(buf+sizeof(Header))));
		game->SetShipAlive(head->id, true);
		std::cout << "Generated ID: " << (int)buf[0] << std::endl;
		if (head->id != -1)
		{
			SendData((struct sockaddr*) &si_other);
		}
		else
		{
			printf("Rejecting client\n");
		}
		//DataUpdate* data = (DataUpdate*)(buf + sizeof(Header));
		//
		//if (data)
		//{
		//	data->data.id = head->id;
		//	data->data.type = PLAYER_SHIP;
		//	objs.push_back(data->data);
		//}
	}
	std::cout << "There is an ID: " << head->id << "\n" << std::endl;
	if (head->cmd == PLAYER_COMMAND)
	{
		PlayerInput* cmd = (PlayerInput*)(buf + sizeof(Header));

		if (*cmd == FIRE)
		{
			BulletData* bullet = (BulletData*)(buf + sizeof(Header) + sizeof(PlayerInput));
			game->FireBullet(bullet->xPos, bullet->yPos, bullet->xVel, bullet->yVel);
		}
		else if (*cmd == LEFT)
		{
			//TODO: THINK ABOUT SENDING THE DATA IN INSTEAD OF DOING IT IN THE METHOD
			UpdateData(head->id, -1, 0, -5);
		}
		else if (*cmd == RIGHT)
		{
			//TODO: THINK ABOUT SENDING THE DATA IN INSTEAD OF DOING IT IN THE METHOD
			UpdateData(head->id, 1, 0, 5);
		}
		else if (*cmd == UP)
		{
			//TODO: THINK ABOUT SENDING THE DATA IN INSTEAD OF DOING IT IN THE METHOD
			UpdateData(head->id, 0, 1, 0);
		}
		else if (*cmd == INPUT_NONE)
		{
			UpdateData(head->id, 0, 0, 0);
		}
		else
		{
			printf("Client sent an illegal command %d\n", *cmd);
		}
	}

	//if (head->cmd == PLAYER_COMMAND)
	//{
	//	PlayerDir* dir = (PlayerDir*)(buf + sizeof(Header));
	//
	//	if (dir)
	//	{
	//		bool foundObj = false;
	//		for (int i = 0; i < objs.size(); i++)
	//		{
	//			if (objs[i].id == head->id)
	//			{
	//				foundObj = true;
	//				if (*dir == LEFT)
	//				{
	//					objs[i].pos.x -= 5;
	//				}
	//				else if (*dir == RIGHT)
	//				{
	//					objs[i].pos.x += 5;
	//				}
	//				else if (*dir == UP)
	//				{
	//					objs[i].pos.y += 5;
	//				}
	//				else
	//				{
	//					objs[i].pos.y -= 5;
	//				}
	//				
	//				break;
	//			}
	//		}
	//
	//		if (!foundObj)
	//		{
	//			ObjData data = ObjData();
	//			data.pos = glm::vec2(0, 0);
	//			data.rot = 0;
	//			data.type = PLAYER_SHIP;
	//			data.id = head->id;
	//			objs.push_back(data);
	//		}
	//	}
	//}
	bufMutex.unlock();

	return EXIT_SUCCESS;
}

void NetworkManager::UpdateData(int id, float xVel, float yVel, float rot)
{
	game->bufferMutex.lock();
	if (id > 7)
	{
		game->lightAccelerationXBuffer[(id % 8)] = xVel;
		game->lightAccelerationYBuffer[(id % 8)] = yVel;
		//memcpy(game->lightAccelerationXBuffer + ((id % 8)*sizeof(float)), &xVel, sizeof(float));
		//memcpy(game->lightAccelerationYBuffer + ((id % 8)*sizeof(float)), &yVel, sizeof(float));
		//memcpy(game->lightRotationBuffer + ((id % 8)*sizeof(float)), &rot, sizeof(float));
	}
	else
	{
		game->shipAccelerationXBuffer[id] = xVel;
		game->shipAccelerationYBuffer[id] = yVel;
		//memcpy(game->shipAccelerationXBuffer + (id*sizeof(float)), &xVel, sizeof(float));
		//memcpy(game->shipAccelerationYBuffer + (id*sizeof(float)), &yVel, sizeof(float));
		//memcpy(game->lightRotationBuffer + (id*sizeof(float)), &rot, sizeof(float));
	}

	game->dirtyBuffers = true;
	game->bufferMutex.unlock();
}

void NetworkManager::ShutDownServer()
{
	runServer = false;
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
			delete m_ptrThread[i];
		}

		break;
	default:
		std::cout << "Unable to close threads " << GetLastError() << std::endl;
	}
}

void NetworkManager::PlayerDied(int id)
{
	bufMutex.lock();

	head->id = id;
	head->cmd = PLAYER_DIED;
	SendToAllClients();

	bufMutex.unlock();
}

//TODO: THERE IS A TODO IN HERE!!!
int NetworkManager::UpdateAllClients()
{
	bufMutex.lock();

	//std::cout << "Pushing to all clients" << std::endl;
	head->cmd = SERVER_UPDATE;
	//TODO: FILL THE BUFFER WITH THE ENTIRE OF EVERYTHING FOR ALL CLIENTS
	int bufferIndex = 0;

	game->physicsMutex.lock();

	float temp[MAX_SHIPS];
	memcpy(temp, game->GetShipPos()->x, sizeof(float) * MAX_SHIPS);

	//Ship Pos
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetShipPos()->x, sizeof(float) * MAX_SHIPS);
	bufferIndex += sizeof(float) * MAX_SHIPS;
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetShipPos()->y, sizeof(float) * MAX_SHIPS);
	bufferIndex += sizeof(float) * MAX_SHIPS;

	//Ship Rot
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetShipPos()->x, sizeof(float) * MAX_SHIPS);
	bufferIndex += sizeof(float) * MAX_SHIPS;
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetShipPos()->y, sizeof(float) * MAX_SHIPS);
	bufferIndex += sizeof(float) * MAX_SHIPS;

	//Ships Alive
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetShipsAlive(), sizeof(bool) * MAX_SHIPS);
	bufferIndex += sizeof(bool) * MAX_SHIPS;

	//Light Pos
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetLightPos()->x, sizeof(float) * MAX_LIGHTS);
	bufferIndex += sizeof(float) * MAX_LIGHTS;
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetLightPos()->y, sizeof(float) * MAX_LIGHTS);
	bufferIndex += sizeof(float) * MAX_LIGHTS;

	//Light Rot
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetLightPos()->x, sizeof(float) * MAX_LIGHTS);
	bufferIndex += sizeof(float) * MAX_LIGHTS;
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetLightPos()->y, sizeof(float) * MAX_LIGHTS);
	bufferIndex += sizeof(float) * MAX_LIGHTS;

	//Asteroid Pos
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetAsteroidPos()->x, sizeof(float) * MAX_ASTEROIDS);
	bufferIndex += sizeof(float) * MAX_ASTEROIDS;
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetAsteroidPos()->y, sizeof(float) * MAX_ASTEROIDS);
	bufferIndex += sizeof(float) * MAX_ASTEROIDS;

	//Asteroid Radius
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetAsteroidRadius()->value, sizeof(float) * MAX_ASTEROIDS);
	bufferIndex += sizeof(float) * MAX_ASTEROIDS;

	//Bullet Pos
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetBulletPos()->x, sizeof(float) * MAX_BULLETS);
	bufferIndex += sizeof(float) * MAX_BULLETS;
	memcpy(buf + sizeof(Header) + bufferIndex, game->GetBulletPos()->y, sizeof(float) * MAX_BULLETS);
	bufferIndex += sizeof(float) * MAX_BULLETS;

	SendToAllClients();

	game->physicsMutex.unlock();

	bufMutex.unlock();

	//std::cout << "Finished sending to all clients" << sizeof(Vec2) << std::endl;
	return EXIT_SUCCESS;
}

//TODO: FIX THIS SO THAT IT TAKES THE TYPE INTO CONSIDERATION. SHIPS ARE 0-7 AND LIGHTS ARE 8-15
//It might be done now
int NetworkManager::GenerateID(ObjType type)
{
	printf("ids %d\n", ids);
	clients.push_back(si_other);

	int typeDisp = 0;

	if (type == PLAYER_LIGHT)
	{
		typeDisp = 8;
	}
	else if (type != PLAYER_SHIP)
	{
		printf("The player is not a correct type.\nExiting\n");
		return -1;
	}

	for (int i = 0 + typeDisp; i < 8 + typeDisp; i++)
	{

		if (!(ids & ((uint64_t)1 << i)))
		{
			ids |= (1 << i);
			return i;
		}
	}

	return -1;
}

void NetworkManager::FreeID(int id)
{
	ids &= ~(1 << id);
	printf("ids after free %d\n", ids);
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i].sin_addr.S_un.S_addr == si_other.sin_addr.S_un.S_addr)
		{
			clients.erase(clients.begin() + i);
		}
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