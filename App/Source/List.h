#pragma once

#include "Core/Core.h"
#include "imgui.h"
#include <nfd.h>
#include <cstdlib>
#include <sstream>

#ifdef _WIN32
#include <direct.h>   // For _mkdir on Windows
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h> // For mkdir on Linux
#define MKDIR(path) mkdir(path, 0777)
#endif

namespace App
{
	struct dataBuffer
	{
		char str1[9] = "";
		char str2[9] = "";
		char str3[16] = "";
		char str4[9] = "";

		std::unordered_map<std::wstring, std::vector<DWORD>> list;

		int item_current_2 = 0;
	};

	struct Timer
	{
		int secs = 0;
		int min = 0;
		int hours = 0;
		int days = 0;
	};

	void init();

	bool folderSetup();

	void clearBuffer();

	void loadBuffer(int);

	void writeAppFileOut(int);

	void readAppFileIn(const char*);

	std::wstring sliceName(const char*);

	std::wstring convertToWString(const char* charArray);

	std::wstring stringToWstring(const std::string& str);

	void convertWStringToCString(const std::wstring& wstr, char* cstr, size_t cstrSize);

	static void HelpMarker(const char* desc);
	
	void renderWindowContext();

	void setTable();

}