#pragma once

#include <Windows.h>

class Thread
{
	public:
		Thread();
		~Thread();
		void Run();
		static DWORD WINAPI ThreadProc(LPVOID Param);
		bool IsFree();
		void CreateWorkerThread();
		HANDLE GetThreadHandle();
		DWORD GetThreadID();
		void SetFunction(void (*callback)());
		void SignalWorkEvent();
		void SignalShutDownEvent();
		void SetThreadBusy();
		void ReleaseHandles();

	private:
		DWORD m_ThreadID;
		HANDLE m_hThread;
		bool m_bIsFree;
		//string m_strMessage;
		void (*m_fCallback)();
		HANDLE m_hWorkEvent[2];
};

