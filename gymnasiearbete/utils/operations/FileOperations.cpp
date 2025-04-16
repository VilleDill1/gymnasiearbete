#include "FileOperations.h"

bool fileHandler::doesFileExist(const std::string& fileName) {
	std::ifstream file(fileName);
	return file.good();
}

std::string fileHandler::loadFile(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file) return "";

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void fileHandler::saveFile(const std::string& fileName, const std::string& content) {
    std::ofstream file(fileName);
    if (!file) return;

    file << content;
    file.close();
}

void fileHandler::createFile(const std::string& fileName) {
    std::ofstream file(fileName);
    file.close();
}