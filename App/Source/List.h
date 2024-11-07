#pragma once

#include "Core/Core.h"
#include "imgui.h"
//#include "simdjson.h"
#include <fstream>
#include <stdio.h>

namespace App
{

	void testData();

	void testLoad();

	// https://github.com/simdjson/simdjson
	//void saveData(const std::string& filename, const simdjson::dom::element& jsonData); 
	
	//simdjson::dom::element loadData(const std::string& filename);

	void convertWStringToCString(const std::wstring& wstr, char* cstr, size_t cstrSize);

	static void HelpMarker(const char* desc);
	
	void renderWindowContext();

	void setTable(std::unordered_map<std::wstring, std::vector<DWORD>>& totalProcessList);

}