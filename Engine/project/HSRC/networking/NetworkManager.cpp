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
	if (networkManager == nullptr)
	{
		return EXIT_FAILURE;
	}

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

NetworkManager::~NetworkManager()
{
	closesocket(s);
	WSACleanup();
}
