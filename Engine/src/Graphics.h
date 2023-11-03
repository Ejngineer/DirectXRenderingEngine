#pragma once
#include "WinInclude.h"
#include "Macros\ExceptionMacros.h"
#include "ExceptionManager.h"
#include "DxgiInfoManager.h"
#include <d3dcompiler.h>
#include <d3d11.h>
#include <wrl.h>

class Graphics
{
public:
	class Exception : public ExceptionManager
	{
		using ExceptionManager::ExceptionManager;
	public:
		Exception(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsg = {}) noexcept;
		const char* what() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public ExceptionManager
	{
		using ExceptionManager::ExceptionManager;
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsg) noexcept;
		const char* what() const noexcept override;
	private:
		std::string info;
	};
public:
	Graphics() = default;
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	~Graphics() = default;
	const Graphics& operator=(const Graphics&) = delete;
	void Clear();
	void DrawTestTriangle();
	void EndFrame();
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	DxgiInfoManager InfoManager;
};