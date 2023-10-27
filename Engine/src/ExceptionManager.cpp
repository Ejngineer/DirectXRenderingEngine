#include "ExceptionManager.h"
#include <sstream>

ExceptionManager::ExceptionManager(int line, const char* file, HRESULT hr) noexcept
	:
	line(line),
	file(file),
	hr(hr)
{}

const char* ExceptionManager::what() const noexcept
{
	std::ostringstream oss;
	oss << "Exception Manager Exception" << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << TranslateErrorCode(hr) << std::endl
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

HRESULT ExceptionManager::GetErrorCode() const noexcept
{
	return hr;
}

std::string ExceptionManager::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree(pMsgBuf);
	return errorString;
}
