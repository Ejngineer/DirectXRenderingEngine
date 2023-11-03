#pragma once
#include "WinInclude.h"
#include <exception>
#include <string>


class ExceptionManager : public std::exception
{
public:
	ExceptionManager(int line, const char* file) noexcept;
	const char* what() const noexcept override;
	int GetLine();
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;
protected:
	int line;
	std::string file;
	mutable std::string whatBuffer;
};