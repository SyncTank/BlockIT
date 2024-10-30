#include "Core.h"

namespace Core {

	void PrintHelloWorld() // Debug test
	{
		std::cout << "Hello World!\n";
		std::cin.get();
	}

	void IterProcess_CPP(std::vector<DWORD>& aProcess) // CPP implementation of function runs to initialize the list
	{
		// Get list of process identifiers
		DWORD cbNeeded, cProcesses;

		if (!EnumProcesses(aProcess.data(), static_cast<DWORD>(aProcess.size() * sizeof(DWORD)), &cbNeeded)) {
			std::cerr << "Failed to enumerate processes." << std::endl;
			return; // Exit the function on failure
		}
    
		// Calculate how many process identifiers were returned
		cProcesses = cbNeeded / sizeof(DWORD);

		aProcess.resize(cProcesses); // resize vector
    
		// Print Name & process identifiers for each process
		//for (const auto& processID : aProcess) {
		//	if (processID != 0 && processID != 3435973836) {
		//		//std::cout << "Process ID: " << processID << std::endl;
		//		PrintProcessAll(processID);
		//	}
		//}
	}

	void IterProcess_C(DWORD* aProcesses, DWORD size) // C implementation of function runs to initialize the list
	{
		// get list of process identifiers
		//DWORD aProcesses[1024];
		DWORD cbNeeded, cProcesses;
		//unsigned int i;

		if (!EnumProcesses(aProcesses, size, &cbNeeded))
		{
			return;
		}

		// Calculate how many pIdentifier were returned
		cProcesses = cbNeeded / sizeof(DWORD);

		//// Print Name & process identifiers for each process
		//for (auto i = 0; i < cProcesses; i++)
		//{
		//	if (aProcesses[i] != 0)
		//	{
		//		PrintProcessAll(aProcesses[i]);
		//	}
		//}

	}

	void PrintProcessAll(DWORD processID) // prints a more detailed list and file path
	{
		TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
		MODULEINFO szInfoBuffer{};
		TCHAR FileNamebuffer[MAX_PATH];

		// get handle of process
		HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
										PROCESS_VM_READ,
										FALSE, processID);
		if (hProcess == NULL)
		{
			std::cout << "Failed to open process, " << GetLastError() << " " << processID << " \n";
			return;
		}
		
		// Get process name
		HMODULE hMod;
		DWORD cbNeeded;

		if ( EnumProcessModulesEx(hProcess, &hMod, sizeof(hMod),
			&cbNeeded, LIST_MODULES_ALL) )
		{
			GetModuleBaseName(hProcess, hMod, szProcessName,
				sizeof(szProcessName) / sizeof(TCHAR));

			GetModuleInformation(hProcess, hMod, &szInfoBuffer,
				sizeof(szInfoBuffer));

			GetModuleFileNameEx(hProcess, NULL, FileNamebuffer, MAX_PATH);

		}

		// Print the process name & identifier
		_tprintf(TEXT("%s (PIDL %u)\n"), szProcessName, processID);
		//std::cout << szInfoBuffer.EntryPoint << "\t" << szInfoBuffer.lpBaseOfDll << "\t" << szInfoBuffer.SizeOfImage << std::endl;
		//std::wcout << "FileName : " << FileNamebuffer;
		

		// Release the handler to process
		CloseHandle(hProcess);
	}

	void ProcessList(std::unordered_map<std::wstring, std::vector<DWORD>>& totalProcessList)
	{
		std::vector<DWORD> processes(1024);
		
		//const auto processSizeBuffer = 1024;
		//DWORD processes[processSizeBuffer];

		Core::IterProcess_CPP(processes);
		//Core::IterProcess_C(processes, processSizeBuffer);

		for (const auto processID : processes)
		{
			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
			MODULEINFO szInfoBuffer{};
			
			// get handle of process
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
				PROCESS_VM_READ,
				FALSE, processID);
			
			if (!hProcess)
			{
				continue;
			}
			
			HMODULE hMod;
			DWORD cbNeeded;
			
			if (EnumProcessModulesEx(hProcess, &hMod, sizeof(hMod),
				&cbNeeded, LIST_MODULES_ALL))
			{
				GetModuleBaseName(hProcess, hMod, szProcessName,
					sizeof(szProcessName) / sizeof(TCHAR));
			}
			
			//_tprintf(TEXT("%s (PIDL %u)\n"), szProcessName, processID);

			totalProcessList[szProcessName].emplace_back(processID);
		}
	}

	void KillProcess(std::vector<DWORD> ids)
	{
		for (const auto pid : ids)
		{
			std::cout << "Something is in the shadows, " << pid << "\n";

			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);

			if (hProcess == NULL)
			{
				std::cout << "Failed to open process," << GetLastError();
				return;
			}

			if (!TerminateProcess(hProcess, 0))
			{
				std::cout << "Failed to terminate process: " << GetLastError() << std::endl;
				std::cout << "HE LIVES!!\n";
			}
			else {
				std::cout << "Process terminated." << std::endl;
				std::cout << "Murder Murder there has been a bloody Murder\n";
			}

			// Release the handler to process
			CloseHandle(hProcess);
		}
	}

}