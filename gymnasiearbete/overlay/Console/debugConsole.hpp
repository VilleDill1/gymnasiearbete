#pragma once
#include <iostream>
#include <chrono>
#include <string>
#include <sstream>
#include <Windows.h>

namespace console
{
	inline bool debug = true;

	__forceinline void allocateConsole() {
		if (AllocConsole()) {
			FILE* fp;
			freopen_s(&fp, "CONOUT$", "w", stdout);
			freopen_s(&fp, "CONOUT$", "w", stderr);
			freopen_s(&fp, "CONIN$", "r", stdin);

			std::cout << "Console Allocated" << std::endl;
		}
		else {
			std::cerr << "Failed to allocate console." << std::endl;
		}
	}

	namespace Logger
	{
		// Manually format the current timestamp
		__forceinline std::string getCurrentTime() {
			auto now = std::chrono::system_clock::now();
			auto now_time_t = std::chrono::system_clock::to_time_t(now);
			std::tm time_info{};
			localtime_s(&time_info, &now_time_t);

			std::ostringstream oss;
			oss << "[" << (time_info.tm_year + 1900) << "-"
				<< (time_info.tm_mon + 1) << "-"
				<< time_info.tm_mday << " "
				<< time_info.tm_hour << ":"
				<< time_info.tm_min << ":"
				<< time_info.tm_sec << "] ";

			return oss.str();
		}

		// Simple log function (no formatting)
		__forceinline void log(const std::string& message) {
			std::cout << getCurrentTime() << message << std::endl;
		}

		// Log function with arguments (using stringstream)
		template <typename... Args>
		__forceinline void log(const std::string& format, const Args&... args) {
			std::ostringstream oss;
			((oss << args << " "), ...); // Fold expression to concatenate all arguments
			std::cout << getCurrentTime() << format << " " << oss.str() << std::endl;
		}
	}
} using namespace console;
