#pragma once

#include "WinInclude.h"
#include "ExceptionManager.h"
#include "Graphics.h"
#include <memory>
#include <optional>

class Window
{
public:
	class WindowException : public ExceptionManager
	{
		using ExceptionManager::ExceptionManager;
	public:
		WindowException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	private:
		HRESULT hr;
	};
public:
	Window() = default;
	Window(int width, int height, const char* title);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window();
	static std::optional<int> ProcessMessage() noexcept;
	Graphics& Gfx();
private:
	HWND hWnd;
	int width;
	int height;
	std::unique_ptr<Graphics> pGfx;
public:
	static LRESULT CALLBACK MsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK MsgInvoke(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
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