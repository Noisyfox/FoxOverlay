#include <cstdio>
#include "framework.h"
#include "FuncExport.h"
#include "Overlay.h"

#define WM_TRIGGER_HOOK (WM_USER + 2019)


LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// Check and start hook thread
	Overlay::instance().startup();

	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static DWORD FindMainThreadId(const DWORD dwProcId)
{
	DWORD dwMainThreadId = 0;

	// Take a snapshot of all threads in the system
	const auto hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap != INVALID_HANDLE_VALUE)
	{
		auto ullMinCreationTime = MAXULONGLONG;

		THREADENTRY32 th32;
		th32.dwSize = sizeof(THREADENTRY32);
		// Enumerate all threads
		for (auto bOk = Thread32First(hThreadSnap, &th32); bOk; bOk = Thread32Next(hThreadSnap, &th32))
		{
			if (th32.th32OwnerProcessID == dwProcId)
			{
				const auto hThread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, th32.th32ThreadID);
				if (hThread)
				{
					// Get the thread creation time
					FILETIME afTimes[4] = {0};
					if (GetThreadTimes(hThread, &afTimes[0], &afTimes[1], &afTimes[2], &afTimes[3]))
					{
						// Convert the FILETIME to a ULONGLONG
						ULARGE_INTEGER uliCreationTime;
						uliCreationTime.HighPart = afTimes[0].dwHighDateTime;
						uliCreationTime.LowPart = afTimes[0].dwLowDateTime;
						const auto ullCreationTime = uliCreationTime.QuadPart;

						if (ullCreationTime > 0ull && ullCreationTime < ullMinCreationTime)
						{
							// Save the min creation time
							ullMinCreationTime = ullCreationTime;

							// The main thread should be created first so it should have the min creation time
							dwMainThreadId = th32.th32ThreadID;
						}
					}

					CloseHandle(hThread);
				}
			}
		}

		CloseHandle(hThreadSnap);
	}

	return dwMainThreadId;
}

bool InstallHook(const DWORD dwProcId, DWORD dwThreadId)
{
	if (dwThreadId == 0)
	{
		printf("dwThreadId == 0, try finding the main thread of process %lu\n", dwProcId);

		dwThreadId = FindMainThreadId(dwProcId);

		if (dwThreadId == 0)
		{
			fprintf(stderr, "Unable to find the main thread of process %lu\n", dwProcId);
			return false;
		}
	}

	printf("Try hooking pid: %lu, thread: %lu\n", dwProcId, dwThreadId);
	const auto hook = SetWindowsHookEx(WH_GETMESSAGE, HookProc, Overlay::instance().getModuleHandle(), dwThreadId);
	if (!hook)
	{
		fprintf(stderr, "SetWindowsHookEx failed, GetLastError = %lu\n", GetLastError());
		return false;
	}

	// Send message to target process to force loading the hook
	for (auto i = 0; i < 5; i++)
	{
		if (!PostThreadMessage(dwThreadId, WM_TRIGGER_HOOK, 0, 0))
		{
			fprintf(stderr, "PostThreadMessage failed, GetLastError = %lu\n", GetLastError());
		}

		Sleep(100);
	}

	return true;
}
