#pragma once

#ifndef _WINDOWS_
#include <Windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#endif

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <locale>
#include <codecvt>

#include <tchar.h>
#include <vector>
#include <unordered_map>

namespace Core {

	void PrintHelloWorld();

	void IterProcess_CPP(std::vector<DWORD>&);

	void IterProcess_C(DWORD*, DWORD);

	void PrintProcessAll(DWORD);

	void ProcessList(std::unordered_map<std::wstring, std::vector<DWORD>>&);

	void GetProcessNames(std::vector<std::wstring>&);

	std::vector<DWORD> GetProcessIDs(const std::wstring& processName);

	static bool IsProcessActive(const std::wstring& processName);

	void KillProcess(std::vector<DWORD>&);

}