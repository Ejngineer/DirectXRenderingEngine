#include "App.h"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR pcmdLine,
	_In_ int nCmdShow
)
{
	App{}.Go();
	return 0;
}