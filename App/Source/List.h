#pragma once

#include "Core/Core.h"
#include "imgui.h"
#include <nfd.h>

#include <algorithm>
#include <thread>
#include <chrono>
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
		char str1[4] = "";
		char str2[6] = "";
		char str3[16] = "";
		char str4[6] = "";

		std::vector<std::wstring> List;

		int item_current_2 = 0;
	};

	struct Timer {
		
		char* nowTime = nullptr;
		char* toTime = nullptr;
		char* timeTill = nullptr;
		bool isRunningClock;

		Timer() 
		{
			isRunningClock = true;
			updateCurrentTime();
			updateTargetTime(0, 0);
		}

		void updateCurrentTime() {
			if (isRunningClock) {  // Only update if running
				auto now = std::chrono::system_clock::now();
				std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
				nowTime = ctime(&currentTime);
			}
		}

		void updateTargetTime(int hours, int minutes) {
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			auto new_time = now + std::chrono::hours(hours) + std::chrono::minutes(minutes);
			std::time_t targetTime = std::chrono::system_clock::to_time_t(new_time);
			toTime = ctime(&targetTime);
		}

		bool checkTimeMatch()
		{
			if (!isRunningClock)
			{
				auto now = std::chrono::system_clock::now();
				std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
				char* currentStrTime = ctime(&currentTime);
				return std::strcmp(toTime, currentStrTime) == 0;
			}
			else
			{
				return false;
			}
		}

	};

	void init(ImGuiIO);

	bool folderSetup();

	void clearBuffer();

	void getToKilling();

	void loadBuffer(int);

	void writeAppFileOut(int);

	void readAppFileIn(const char*);

	std::wstring sliceName(const char*);

	std::wstring convertToWString(const char*);

	std::wstring stringToWstring(const std::string&);

	void convertWStringToCString(const std::wstring&, char*, size_t);

	static void startNameThreadProcess(std::vector<std::wstring>&);

	static void startThreadProcessToKill(std::vector<std::wstring>& process);

	static void HelpMarker(const char*);

	bool is_digits(char*);
	
	void renderWindowContext();

	void setTable();

}