#ifndef UNICODE
#define UNICODE
#endif

#include "App.h"
#include "ExceptionManager.h"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR lpCmdLine,
	_In_ int nCmdShow
)
{
	try
	{
		App{}.Go();
	}
	catch (const ExceptionManager& e)
	{
		MessageBoxA(nullptr, e.what(), "Engine Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return 0;
}