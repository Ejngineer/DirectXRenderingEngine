#include "Window.h"
#include "Macros/ExceptionMacros.h"
#include <sstream>

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

	//Calculate window size based on style parameters
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

	pGfx = std::make_unique<Graphics>(hWnd);
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

Graphics& Window::Gfx()
{
	if (pGfx == nullptr)
	{
		throw ENGINE_LASTEXCEPT();
	}
	return *pGfx;
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
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::MsgInvoke));

		//invoke our class handler
		return pWnd->WndProc(hWnd, uMsg, wParam, lParam);
	}
	
	//return default handler if we get a message before WM_CREATE
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::MsgInvoke(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window* pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return 
		pWnd->WndProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
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
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = Window::MsgSetup;
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

Window::WindowException::WindowException(int line, const char* file, HRESULT hr) noexcept
	:
	ExceptionManager(line, file),
	hr(hr)
{}

const char* Window::WindowException::what() const noexcept
{
	std::ostringstream oss;
	oss << "Graphics Exception" << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << TranslateErrorCode(hr) 
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

HRESULT Window::WindowException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::WindowException::TranslateErrorCode(HRESULT hr) noexcept
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
