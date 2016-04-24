#include "Thread.h"

#include <iostream>

Thread::Thread()
{
	m_ThreadID = 0;
	m_hThread = NULL;
	m_bIsFree = true;

	m_hWorkEvent[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hWorkEvent[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
}


Thread::~Thread()
{
}

DWORD WINAPI Thread::ThreadProc(LPVOID Param)
{
	Thread* ptrThread = (Thread*)Param;
	bool bShutDown = false;

	while (!bShutDown)
	{
		DWORD dwWaitResult = WaitForMultipleObjects(2, ptrThread->m_hWorkEvent, FALSE, INFINITE);

		switch (dwWaitResult)
		{
			case WAIT_OBJECT_0:
				ptrThread->Run();
				break;

			case WAIT_OBJECT_0+1:
				bShutDown = true;
				break;
		}
	}

	return 0;
}

void Thread::Run()
{
	//Add the call function code here
	m_fCallback();

	m_bIsFree = true;
	ResetEvent(m_hWorkEvent[0]);
}

bool Thread::IsFree()
{
	return m_bIsFree;
}

void Thread::CreateWorkerThread()
{
	m_hThread = CreateThread(NULL, NULL, ThreadProc, (LPVOID)this, NULL, &m_ThreadID);

	if (m_hThread == NULL)
	{
		std::cout << "Thread could not be created" << GetLastError() << std::endl;
	}
	else
	{
		std::cout << "Thread " << m_ThreadID << " created successfully" << std::endl;
	}
}

void Thread::SetFunction(void (*callback)())
{
	m_fCallback = callback;
}

HANDLE Thread::GetThreadHandle()
{
	return m_hThread;
}

DWORD Thread::GetThreadID()
{
	return m_ThreadID;
}

void Thread::SignalWorkEvent()
{
	SetEvent(m_hWorkEvent[0]);
}

void Thread::SignalShutDownEvent()
{
	SetEvent(m_hWorkEvent[1]);
}

void Thread::SetThreadBusy()
{
	m_bIsFree = false;
}

void Thread::ReleaseHandles()
{
	CloseHandle(m_hThread);
	CloseHandle(m_hWorkEvent[0]);
	CloseHandle(m_hWorkEvent[1]);
}