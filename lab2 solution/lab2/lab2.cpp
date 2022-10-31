#ifndef UNICODE
#define UNICODE
#endif 

#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

void SuspendProcess(DWORD processId)
{
	HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	THREADENTRY32 threadEntry;
	threadEntry.dwSize = sizeof(THREADENTRY32);

	Thread32First(hThreadSnapshot, &threadEntry);

	do
	{
		if (threadEntry.th32OwnerProcessID == processId)
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE,
				threadEntry.th32ThreadID);

			SuspendThread(hThread);
			CloseHandle(hThread);
		}
	} while (Thread32Next(hThreadSnapshot, &threadEntry));

	CloseHandle(hThreadSnapshot);
}

int main()
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!hSnap)
	{
		std::cout << "Can't get handle of snapshot process!" << std::endl;
		return 1;
	}

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);

	HANDLE process = { 0 };

	std::vector<std::pair<DWORD, std::pair<WCHAR*, DWORD>>> p;

	Process32First(hSnap, &pe);
	do
	{
		//wprintf(L"%s\t\t\t\t%d\t%d\n",pe.szExeFile, pe.th32ProcessID, pe.cntThreads);
		if (!wcscmp(pe.szExeFile, L"winAPI_Console.exe"))
		{
			process = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pe.th32ProcessID);
		}
		std::pair<DWORD, std::pair<WCHAR*, DWORD>> temp;
		temp.first = pe.cntThreads;
		temp.second.first = pe.szExeFile;
		temp.second.second = pe.th32ProcessID;
		p.push_back(temp);

	} while (Process32Next(hSnap, &pe));
	CloseHandle(hSnap);

	std::sort(p.begin(), p.end());
	std::reverse(p.begin(), p.end());


	for (int i = 0; i < 5; i++)
	{
		wprintf(L"%s", p[i].second.first);
		std::cout << "\t\t\t\ " << p[i].second.second << "\t" << p[i].first << std::endl;
	}

	while (true)
	{
		std::cout << "Enter command: ";
		std::string command;
		std::cin >> command;
		std::cout << "You entered: " << command << std::endl;

		if (command == "high")
		{
			SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
			std::cout << "The priority has changed to high" << std::endl;
		}
		else if (command == "idle")
		{
			SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
			std::cout << "The priority has changed to idle" << std::endl;
		}
		else if (command == "normal")
		{
			SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
			std::cout << "The priority has changed to normal" << std::endl;
		}
		else if (command == "realtime")
		{
			SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
			std::cout << "The priority has changed to realtime" << std::endl;
		}
		else if (command == "exit")
		{
			std::cout << "Exit process" << std::endl;
			ExitProcess(0);
		}
		else if (command == "terminate")
		{
			std::cout << "Terminate process" << std::endl;
			TerminateProcess(GetCurrentProcess(), 0);
		}
		else if (command == "suspend")
		{
			std::cout << "Suspend process" << std::endl;
			SuspendProcess(GetCurrentProcessId());
		}
		else
		{
			std::cout << "You entered wrong command" << std::endl;
		}
	}

	if (process)
	{
		TerminateProcess(process, 0);
		CloseHandle(process);
	}

	return 0;
}