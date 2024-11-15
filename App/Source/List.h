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
		char* timeLeft = nullptr;
		bool isRunningClock;
		std::chrono::system_clock::time_point now;
		std::chrono::system_clock::time_point new_Time;

		Timer() 
		{
			isRunningClock = true;
			updateCurrentTime();
			updateTargetTime(0, 0);
		}

		// #TODO either remove or rework the method calls
		// fixed issue so timers are now functional apart from each other
		void updateCurrentTime() { 
			if (isRunningClock) {  // Only update if running
				now = std::chrono::system_clock::now();
				std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
				const char* currentTimeStr = ctime(&currentTime);

				// Allocate memory for nowTime and copy the string
				nowTime = new char[strlen(currentTimeStr) + 1];
				strcpy_s(nowTime, strlen(currentTimeStr) + 1, currentTimeStr);
			}
		}

		void updateTime() { // dup for test
				now = std::chrono::system_clock::now();
				std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
				const char* currentTimeStr = ctime(&currentTime);

				// Allocate memory for nowTime and copy the string
				nowTime = new char[strlen(currentTimeStr) + 1];
				strcpy_s(nowTime, strlen(currentTimeStr) + 1, currentTimeStr);
		}

		void updateTargetTime(int hours, int minutes) {
			new_Time = now + std::chrono::hours(hours) + std::chrono::minutes(minutes);
			std::time_t targetTime = std::chrono::system_clock::to_time_t(new_Time);
			const char* targetTimeStr = ctime(&targetTime);

			toTime = new char[strlen(targetTimeStr) + 1];
			strcpy_s(toTime, strlen(targetTimeStr) + 1, targetTimeStr);
		}

		void updateTimeLeft() 
		{
			auto x = new_Time - now;
			std::chrono::duration<double> elapsed_seconds = new_Time - now;
			std::string elasped_str = std::to_string(elapsed_seconds.count());

			timeLeft = new char[elasped_str.length() + 1];
			strcpy_s(timeLeft, elasped_str.length() + 1, elasped_str.c_str());
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

	static void getToKilling();

	static void HelpMarker(const char*);

	bool is_digits(char*);
	
	void renderWindowContext();

	void setTable();

}