#include "Window.h"
#include "Macros/ExceptionMacros.h"

WindowClass WindowClass::wndClass;

Window::Window(int width, int height, const char* title)
	:
	width(width),
	height(height)
{
	//make sure the client region is the size we specify in code
	RECT wr = { 0 };
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;

	//Calculate window size bassed on style paramaters
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZE | WS_SYSMENU, FALSE) == 0)
		throw ENGINE_LASTEXCEPT();

	hWnd = CreateWindow
	(
		WindowClass::GetName(),
		title,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		WindowClass::GetInstance(),
		this //custom parameter so that the window contains a pointer to our window class
	);

	if (hWnd == nullptr)
		throw ENGINE_LASTEXCEPT();

	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

std::optional<int> Window::ProcessMessage() noexcept
{
	MSG msg = { 0 };
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return (int)msg.wParam;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

LRESULT WINAPI Window::MsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		/*Extract pointer from CREATESTRUCT that stores a pointer to our window class passed in
		at CreateWindow call*/
		const CREATESTRUCT* const pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		Window* pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		//Set winapi user data to store a pointer to our window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

		//Set winapi wndproc to store pointer to our user defined WndProc function after initial setup
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::WndProc));

		//invoke our class handler
		return pWnd->WndProc(hWnd, uMsg, wParam, lParam);
	}
	
	//return default handler if we get a message before WM_CREATE
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT WINAPI Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = Window::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetInstance();
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = GetName();
	wcex.hIconSm = NULL;

	RegisterClassEx(&wcex);
}

WindowClass::~WindowClass()
{
	UnregisterClass(GetName(), GetInstance());
}

const char* WindowClass::GetName() noexcept
{
	return CLASS_NAME;
}

HINSTANCE WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}






