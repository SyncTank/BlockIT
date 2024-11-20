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
		char* timeStart = nullptr;
		std::chrono::duration<double> elapsed_timeLeft;
		std::chrono::system_clock::time_point now;
		std::chrono::system_clock::time_point new_Time;

		Timer() 
		{
			updateCurrentTime();
			updateTargetTime(0, 0);
		}

		void updateCurrentTime() { // dup for test
				now = std::chrono::system_clock::now();
				std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

				char buffer[30];
				ctime_s(buffer, sizeof(buffer), &currentTime);

				// Allocate memory for nowTime and copy the string
				nowTime = new char[strlen(buffer) + 1];
				strcpy_s(nowTime, strlen(buffer) + 1, buffer);
		}

		void updateTargetTimeMilitary(int hours, int minutes) {
			// Convert to time_t to manipulate the time
			std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

			// Buffer to hold the tm structure
			std::tm now_tm;

			// Convert to tm structure
			// Use localtime_s for safer conversion
			localtime_s(&now_tm, &now_time_t);

			// Set the hour, minute, second to 0 to get 0000 hour
			now_tm.tm_hour = hours;
			now_tm.tm_min = minutes;

			// Convert back to time_t
			std::time_t new_time_t = std::mktime(&now_tm);

			// Convert back to chrono time_point
			auto zerod = std::chrono::system_clock::from_time_t(new_time_t);
			auto n_time = std::chrono::system_clock::to_time_t(zerod);
			// Buffer to hold the formatted time string
			char buffer[30];
			ctime_s(buffer, sizeof(buffer), &n_time);

			toTime = new char[strlen(buffer) + 1];
			strcpy_s(toTime, strlen(buffer) + 1, buffer);
		}

		void updateTargetTime(int hours, int minutes) {
			new_Time = now + std::chrono::hours(hours) + std::chrono::minutes(minutes);
			std::time_t targetTime = std::chrono::system_clock::to_time_t(new_Time);
			char buffer[30];
			ctime_s(buffer, sizeof(buffer), &targetTime);

			toTime = new char[strlen(buffer) + 1];
			strcpy_s(toTime, strlen(buffer) + 1, buffer);
		}

		void updateStartTimeMilitary(int hours, int minutes) {
			// Convert to time_t to manipulate the time
			std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

			// Buffer to hold the tm structure
			std::tm now_tm;

			// Convert to tm structure
			// Use localtime_s for safer conversion
			localtime_s(&now_tm, &now_time_t);

			// Set the hour, minute, second to 0 to get 0000 hour
			now_tm.tm_hour = hours;
			now_tm.tm_min = minutes;

			// Convert back to time_t
			std::time_t new_time_t = std::mktime(&now_tm);

			// Convert back to chrono time_point
			auto zerod = std::chrono::system_clock::from_time_t(new_time_t);
			auto n_time = std::chrono::system_clock::to_time_t(zerod);
			char buffer[30];
			ctime_s(buffer, sizeof(buffer), &n_time);

			timeStart = new char[strlen(buffer) + 1];
			strcpy_s(timeStart, strlen(buffer) + 1, buffer);
		}

		void updateStartTime(int hours, int minutes) {
			new_Time = now + std::chrono::hours(hours) + std::chrono::minutes(minutes);
			std::time_t targetTime = std::chrono::system_clock::to_time_t(new_Time);
			char buffer[30];
			ctime_s(buffer, sizeof(buffer), &targetTime);

			timeStart = new char[strlen(buffer) + 1];
			strcpy_s(timeStart, strlen(buffer) + 1, buffer);
		}

		bool compareStartTime() const
		{
			if (std::strcmp(nowTime, timeStart) == 0)
			{
				return true;
			}
			return false;
		}

		bool isVaildTime(int hr, int min) const
		{
			// Convert to time_t to manipulate the time
			std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

			// Buffer to hold the tm structure
			std::tm now_tm;

			// Convert to tm structure
			// Use localtime_s for safer conversion
			localtime_s(&now_tm, &now_time_t);

			// Set the hour, minute, second to 0 to get 0000 hour
			now_tm.tm_hour = hr;
			now_tm.tm_min = min;

			// Convert back to time_t
			std::time_t new_time_t = std::mktime(&now_tm);

			// Convert back to chrono time_point
			auto zerod = std::chrono::system_clock::from_time_t(new_time_t);
			auto n_time = std::chrono::system_clock::to_time_t(zerod);

			auto duration_since_epoch = zerod.time_since_epoch();
			auto seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(duration_since_epoch).count();

			auto now_since = now.time_since_epoch();
			auto seconds_now_epoch = std::chrono::duration_cast<std::chrono::seconds>(now_since).count();
			//const char* testTime = ctime(&say);

			/*	Test Cases
				1711 1833
				1711 1833 2100 2300
				0300 1100 1711 2100
			*/

			if (seconds_since_epoch < seconds_now_epoch)//1731938411  1731956380 
			{
				return false;
			}
			return true;
		}

		void updateTimeLeft() 
		{
			elapsed_timeLeft = new_Time - now;
			std::string elapsed_str = std::to_string(elapsed_timeLeft.count());

			timeLeft = new char[elapsed_str.length() + 1];
			strcpy_s(timeLeft, elapsed_str.length() + 1, elapsed_str.c_str());
		}

	};

	void init(ImGuiIO);

	bool getIsRunning();

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