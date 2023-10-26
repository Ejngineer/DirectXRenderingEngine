#pragma once

#include <windows.h>
#include <optional>

class Window
{
public:
	Window() = default;
	Window(int width, int height, const char* title);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window();
	static std::optional<int> ProcessMessage() noexcept;
private:
	HWND hWnd;
	int width;
	int height;
public:
	static LRESULT CALLBACK MsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class WindowClass
{
public:
	static const char* GetName() noexcept;
	static HINSTANCE GetInstance() noexcept;
	WindowClass() noexcept;
	~WindowClass();
	WindowClass(const WindowClass&) = delete;
	WindowClass& operator=(const WindowClass&) = delete;
private:
	static constexpr const char* CLASS_NAME = "DirectX Window";
	HINSTANCE hInst;
	static WindowClass wndClass;
};