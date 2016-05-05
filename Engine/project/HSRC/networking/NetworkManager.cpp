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

	//start communication
	//printf("start client : ");
	//gets_s(message);
	recv_len = sizeof(Header);
	//data = (bufferData*)buf;

	memset(buf, '\0', BUFLEN);
	head->id = 0;
	head->cmd = UPDATE;

	DataUpdate* data = (DataUpdate*)(buf + sizeof(Header));
	data->numObj = 1;
	//data->data = &ObjData();
	//data->data = (ObjData*)(data + sizeof(DataUpdate));
	//data
	int num = sizeof(Header);
	int num2 = sizeof(Header*);
	//data->data->pos.push_back

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
	//id = (int*)&buf[0];
	printf("Id of Client: %d\n", head->id);
	//int* xData = (int*)&buf[8];
	//int* yData = (int*)&buf[16];
	printf("size of int* %d\n", sizeof(int*));
	//xPos = *xData;
	//yPos = *yData;
	//printf("xPos: %d\n", data->xPos);
	//printf("yPos: %d\n", data->yPos);
	timeval tv;
	fd_set fds;
	while (runClient)
	{
		//printf("Waiting for data...\n");
		fflush(stdout);

		bufMutex.lock();
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
	//int* bufPoint = (int*)&buf[0];
	//bufPoint = id;
	head->id = id;
	printf("Sending ID: %d\n", head->id);
	//printf("Status: %d\n", data->alive);
	//printf("Firing: %d\n", data->fired);
	//printf("Connected: %d\n", data->connected);

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

	if (head && head->id != 0)
	{
		id = head->id;
	}

	if (head->cmd == UPDATE)
	{
		DataUpdate* data = (DataUpdate*)(buf + sizeof(Header));
		if (data->data.id == id)
		{
			ship = data->data;
		}
		else
		{
			bool inVector = false;
			for (int i = 0; i < otherObjs.size(); i++)
			{
				if (otherObjs[i].id == data->data.id)
				{
					inVector = true;
					otherObjs[i] = data->data;
					break;
				}
			}

			if (!inVector)
			{
				otherObjs.push_back(data->data);
			}
		}
	}

	//print details of the client/peer and the data received
	std::cout << "Data ID: " << head->id << std::endl;
	std::cout << "Client ID: " << id << std::endl;

	bufMutex.unlock();
	
	printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	printf("Data: %s\n", buf);
	
}

void NetworkManager::updateData(glm::vec3 pos, glm::vec3 rot)
{
	//threadManager->CreateWorkerThread();
	//int* bufPoint = (int*)&buf[0];
	//bufPoint = id;
	bufMutex.lock();
	DataUpdate* data = (DataUpdate*)(buf + sizeof(Header));
	int num = sizeof(Header);
	
	ship.pos = (glm::vec2)pos;
	ship.rot = (rot.x);
	data->data = ship;
	//vector<ObjData> temp;
	//temp.push_back(ObjData((glm::vec2)pos, rot.x, PLAYER_SHIP));
	//data->data = temp;
	//data->data.push_back
	//data->datapos.push_back((glm::vec2)pos);
	//data->data->rot.push_back(rot.x);

	/*if (btn == 'i')
	{
		xPos = data->pos->x;
		yPos = data->pos->y;
		printf("i pressed");
	}
	if (btn == 'j')
	{
		xPos = data->pos->x;
		yPos = data->pos->y;
		printf("j pressed");
	}
	if (btn == 'k')
	{
		xPos = data->pos->x;
		yPos = data->pos->y;
		printf("k pressed");
	}
	if (btn == 'l')
	{
		xPos = data->pos->x;
		yPos = data->pos->y;
		printf("l pressed");
	}*/

	head->cmd = UPDATE;
	sendData();

	bufMutex.unlock();
	//receiveData();
	printf("updating Data");
}

void NetworkManager::serverUpdate()
{
	//threadManager->CreateWorkerThread();
	//int* bufPoint = (int*)&buf[0];
	//bufPoint = id;
	bufMutex.lock();

	

	sendData();
	bufMutex.unlock();
	//receiveData();
	printf("updating Data");
}

void NetworkManager::died()
{
	bufMutex.lock();

	//data->alive = true;
	head->cmd = PLAYER_DIED;

	sendData();
	bufMutex.unlock();
}

void NetworkManager::fired()
{
	bufMutex.lock();

	//data->fired = true;
	head->cmd = BULLET_FIRED;

	sendData();
	bufMutex.unlock();
	//data->fired = false;
}

void NetworkManager::clientDisconnect()
{
	bufMutex.lock();

	//data->connected = false;
	head->cmd = PLAYER_DISCONNECT;

	sendData();
	bufMutex.unlock();
}

void NetworkManager::shutDownClient()
{
	runClient = false;
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
			delete m_ptrThread[i]; //THIS NEEDS TO BE FIXED SO THAT IT DOESN"T DESTROY THE THREADS
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
