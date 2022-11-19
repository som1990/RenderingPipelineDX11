#include <RenderingPipelineDX11PCH.h>
#include "Graphics.h"

namespace wrl = Microsoft::WRL;
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__,__FILE__,hr)
#define GFX_THROW_IF_NOINFO(hrcall) if(FAILED(hr=(hrcall))) throw Graphics::HrException(__LINE__,__FILE__,hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__,__FILE__,(hr), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED(hr=(hrcall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemoveException(__LINE__,__FILE__, hr, infoManager.GetMessages())
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_IF_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemoveException( __LINE__,__FILE__,(hr) )
#endif //NDEBUG
DXGI_RATIONAL QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync)
{
	DXGI_RATIONAL refreshRate = { 0, 1 };

	if (vsync)
	{
		wrl::ComPtr<IDXGIFactory> factory;
		wrl::ComPtr<IDXGIAdapter> adapter;
		wrl::ComPtr<IDXGIOutput> adapterOutput;
		DXGI_MODE_DESC* displayModeList;

		// Create a DirectX graphics interface factory
		HRESULT gfxFactoryResult = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(gfxFactoryResult))
		{
			MessageBox(
				0,
				TEXT("Could not create DXGIFactory Instance."),
				TEXT("Query Refresh Rate"),
				MB_OK
			);

			throw std::exception("Failed to create DXGIFactory.");
		}

		gfxFactoryResult = factory->EnumAdapters(0, &adapter);
		if (FAILED(gfxFactoryResult))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapters."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw std::exception("Failed to enumerate adapters.");
		}
		gfxFactoryResult = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(gfxFactoryResult))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapter outputs."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw std::exception("Failed to enumerate adapter outputs.");
		}

		UINT numDisplayModes = {};
		gfxFactoryResult = adapterOutput->GetDisplayModeList(
			DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr
		);

		if (FAILED(gfxFactoryResult))
		{
			MessageBox(0,
				TEXT("Failed to query display mode list."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw std::exception("Failed to query display mode list.");
		}


		displayModeList = new DXGI_MODE_DESC[numDisplayModes];
		assert(displayModeList);

		gfxFactoryResult = adapterOutput->GetDisplayModeList(
			DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, displayModeList
		);

		if (FAILED(gfxFactoryResult))
		{
			MessageBox(0,
				TEXT("Failed to query display mode list."),
				TEXT("Query Refresh Rate"),
				MB_OK);
			delete[] displayModeList;
			throw std::exception("Failed to query display mode list.");
		}

		// Store the refresh rate of the monitor that matches the width and height of the requested screen.
		for (UINT i = 0; i < numDisplayModes; ++i)
		{
			if (displayModeList[i].Width != screenWidth || displayModeList[i].Height != screenHeight)
			{
				continue;
			}
			refreshRate = displayModeList[i].RefreshRate;
		}
		delete[] displayModeList;
	}

	return refreshRate;
}
/*
	Initialize the DirectX device and swap chain

	The process of initializing a Direct3D rendering device consists of several steps:

	1) Create the device and swap chain,
	2) Create a render target view of the swap chain’s back buffer,
	3) Create a texture for the depth-stencil buffer,
	4) Create a depth-stencil view from the depth-stencil buffer,
	5) Create a depth-stencil state object that defines the behaviour of the output merger stage,
	6) Create a rasterizer state object that defines the behaviour of the rasterizer stage.
*/
Graphics::Graphics(HWND hWnd, int clientWidth, int clientHeight, BOOL vSync)
	: clientWidth(clientWidth), clientHeight(clientHeight), vSync(vSync)
{
	// 1) Create Device and swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2; // 2 Buffers (2-1 = 1) 
	swapChainDesc.BufferDesc.Width = clientWidth;
	swapChainDesc.BufferDesc.Height = clientHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	//swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	//swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	swapChainDesc.BufferDesc.RefreshRate = QueryRefreshRate(clientWidth, clientHeight, vSync);
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1; // Antialiasing Count
	swapChainDesc.SampleDesc.Quality = 0; // Antialiasing Quality
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Windowed = TRUE;
	
	UINT createDeviceFlags = 0;
#ifndef NDEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	D3D_FEATURE_LEVEL featureLevel;
	
	HRESULT hr;
	GFX_THROW_INFO( D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		featureLevels,
		_countof(featureLevels),
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&pSwap,
		&pDevice,
		&featureLevel,
		&pContext
	));
	
	// 2) Create a render target view of the swap chain’s back buffer
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRenderTargetView));

}


void Graphics::EndFrame()
{	
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (vSync)
	{
		hr = pSwap->Present(1u, 0u);
	}
	else
	{
		hr = pSwap->Present(0u, 0u);
	}
	if FAILED(hr)
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

// Graphics Exceptions
Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	: Exception(line, file), hr(hr)
{
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	//remove final newline if exists
	if(!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
			<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
			<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
			<< "[Error String]" << GetErrorString() << std::endl
			<< "[Description]" << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "SGD3D Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	std::wstring wideString = DXGetErrorString(hr);
	return std::string(wideString.begin(), wideString.end());
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	wchar_t buf[4098];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	std::wstring wideString = buf;
	return std::string(wideString.begin(), wideString.end());
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemoveException::GetType() const noexcept
{
	return "SGD3D Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED";
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	: Exception(line, file)
{
	for (const auto& m : infoMsgs)
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
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();

}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "SGD3D Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
