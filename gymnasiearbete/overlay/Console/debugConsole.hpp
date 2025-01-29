#pragma once
#include <Windows.h>
#include <cstdio>
#include <iostream>

namespace console 
{
	inline bool debug = true;

	inline void allocateConsole() {
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
}