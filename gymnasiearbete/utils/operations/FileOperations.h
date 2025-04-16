#pragma once

#include <Windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <ostream>

namespace fileHandler
{
	bool doesFileExist(const std::string& fileName);
	std::string loadFile(const std::string& fileName);
	void saveFile(const std::string& fileName, const std::string& content);
	void createFile(const std::string& fileName);
}