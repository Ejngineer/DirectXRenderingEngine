#include "Graphics.h"
#include <sstream>
#include <vector>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3Dcompiler.lib")

namespace wrl = Microsoft::WRL;

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = 1920;
	swapChainDesc.BufferDesc.Height = 1080;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1u;
	swapChainDesc.SampleDesc.Quality = 0u;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2u;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = 0u;

	UINT swapChainFlags = D3D11_CREATE_DEVICE_DEBUG;
	HRESULT hr;
	ENGINE_HREXCEPT_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		swapChainFlags,
		nullptr,
		0u,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&pSwapChain,
		&pDevice,
		NULL,
		&pContext
	));

	wrl::ComPtr<ID3D11Resource> pbackBuffer;
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;

	ENGINE_HREXCEPT_NO_INFO(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pbackBuffer));
	ENGINE_HREXCEPT_NO_INFO(pDevice->CreateRenderTargetView(pbackBuffer.Get(), NULL, &pTarget));
}

void Graphics::Clear()
{
	const FLOAT clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), clearColor);
}

void Graphics::DrawTestTriangle()
{
	HRESULT hr;
	struct Vertex
	{
		float x;
		float y;
	};

	Vertex vertices[] =
	{
		{0.0f, 0.5f},
		{0.5f, -0.5f},
		{-0.5f, -0.5f},
	};

	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertices;

	UINT stride = sizeof(Vertex);
	UINT offset = 0u;
	wrl::ComPtr<ID3D11Buffer> pBuffer;
	ENGINE_HREXCEPT_INFO(pDevice->CreateBuffer(&bd, &sd, &pBuffer));
	pContext->IASetVertexBuffers(0u, 1u, pBuffer.GetAddressOf(), &stride, &offset);

	wrl::ComPtr<ID3DBlob> pBlob;
	ENGINE_HREXCEPT_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	ENGINE_HREXCEPT_INFO(pDevice->CreatePixelShader(
		pBlob->GetBufferPointer(), 
		pBlob->GetBufferSize(), 
		nullptr, 
		&pPixelShader
	));
	pContext->PSSetShader(pPixelShader.Get(), NULL, 0u);

	ENGINE_HREXCEPT_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	ENGINE_HREXCEPT_INFO(pDevice->CreateVertexShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&pVertexShader
	));
	pContext->VSSetShader(pVertexShader.Get(), NULL, 0u);

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u}
	};

	wrl::ComPtr<ID3D11InputLayout> pLayout;
	ENGINE_HREXCEPT_INFO(pDevice->CreateInputLayout(
		ied,
		(UINT)std::size(ied),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pLayout
	));
	pContext->IASetInputLayout(pLayout.Get());

	pContext->OMSetRenderTargets(1, pTarget.GetAddressOf(), nullptr);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_VIEWPORT vp{};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = 1920.0f;
	vp.Height = 1080.0f;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;

	pContext->RSSetViewports(1u, &vp);


	ENGINE_HREXCEPT_INFO_ONLY(pContext->Draw((UINT)std::size(vertices), 0u));
}

void Graphics::EndFrame()
{
	InfoManager.set();
	HRESULT hr = NULL;
	if (FAILED(pSwapChain->Present(1, 0)))
	{
		ENGINE_HREXCEPT_NO_INFO(hr);
	}
}

Graphics::Exception::Exception(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsg) noexcept
	:
	ExceptionManager(line, file),
	hr(hr)
{
	for (auto& m : infoMsg)
	{
		info += m;
		info.push_back('\n');
	}
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << "Graphics Exception" << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << TranslateErrorCode(hr) << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << info << std::endl << std::endl;;
	}
	oss	<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

HRESULT Graphics::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::Exception::TranslateErrorCode(HRESULT hr) noexcept
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

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsg) noexcept
	:
	ExceptionManager(line, file)
{
	for (auto& m : infoMsg)
	{
		info += m;
		info.push_back('\n');
	}

	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << "Info Only Exception" << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << info << std::endl << std::endl;;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}
