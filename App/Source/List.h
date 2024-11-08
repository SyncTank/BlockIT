#pragma once

#include "Core/Core.h"
#include "imgui.h"
#include <nfd.h>

#ifdef _WIN32
#include <direct.h>   // For _mkdir on Windows
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h> // For mkdir on Linux
#define MKDIR(path) mkdir(path, 0777)
#endif


//#include "simdjson.h"

namespace App
{
	struct databuffers {
		static char str1[];
		static char str2[];
		static char str3[];
		static char str4[];

		static int item_current_2;
	};

	void init();

	bool folderSetup();

	void writeAppFileOut();

	void readAppFileIn(const char*);

	std::wstring sliceName(const char*);

	// https://github.com/simdjson/simdjson
	//void saveData(const std::string& filename, const simdjson::dom::element& jsonData); 
	
	//simdjson::dom::element loadData(const std::string& filename);

	std::wstring convertToWString(const char* charArray);

	void convertWStringToCString(const std::wstring& wstr, char* cstr, size_t cstrSize);

	static void HelpMarker(const char* desc);
	
	void renderWindowContext(int);

	void setTable(std::unordered_map<std::wstring, std::vector<DWORD>>& totalProcessList);

}