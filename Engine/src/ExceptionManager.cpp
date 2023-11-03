#include "ExceptionManager.h"
#include <sstream>

ExceptionManager::ExceptionManager(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

const char* ExceptionManager::what() const noexcept
{
	std::ostringstream oss;
	oss << "Exception Manager Exception" << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

int ExceptionManager::GetLine()
{
	return line;
}

const std::string& ExceptionManager::GetFile() const noexcept
{
	return file;
}

std::string ExceptionManager::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}
