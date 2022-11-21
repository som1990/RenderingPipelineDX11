#include <RenderingPipelineDX11PCH.h>
#include "Graphics.h"

namespace wrl = Microsoft::WRL;
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__,__FILE__,hr)
#define GFX_THROW_IF_NOINFO(hrcall) if(FAILED(hr=(hrcall))) throw Graphics::HrException(__LINE__,__FILE__,hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__,__FILE__,(hr), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED(hr=(hrcall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemoveException(__LINE__,__FILE__, hr, infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException(__LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_IF_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemoveException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
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
	2) Create a render target view of the swap chain�s back buffer,
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
	
	// 2) Create a render target view of the swap chain�s back buffer
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pRenderTargetView));

	// 3) Create a texture for the depth-stencil bufer
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc = {};
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0; // No CPU access required
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.Width = clientWidth;
	depthStencilBufferDesc.Height = clientHeight;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

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

void Graphics::DrawTestTriangle()
{
	{
		namespace wrl = Microsoft::WRL;
		namespace DX11 = DirectX;
		struct VertexPosColor
		{
			DX11::XMFLOAT3 Position;
			DX11::XMFLOAT3 Color;
		};
		//Create Vertex buffer
		/*
		VertexPosColor g_Vertices[8] =  // CUBE
		{
			{ DX11::XMFLOAT3(-1.0f, -1.0f, -1.0f), DX11::XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
			{ DX11::XMFLOAT3(-1.0f,  1.0f, -1.0f), DX11::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
			{ DX11::XMFLOAT3(1.0f,  1.0f, -1.0f), DX11::XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
			{ DX11::XMFLOAT3(1.0f, -1.0f, -1.0f), DX11::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
			{ DX11::XMFLOAT3(-1.0f, -1.0f,  1.0f), DX11::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
			{ DX11::XMFLOAT3(-1.0f,  1.0f,  1.0f), DX11::XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
			{ DX11::XMFLOAT3(1.0f,  1.0f,  1.0f), DX11::XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
			{ DX11::XMFLOAT3(1.0f, -1.0f,  1.0f), DX11::XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
		};
		*/
		
		VertexPosColor g_Vertices[] = //TRIANGLE or Trapezoid
		{
			{ DX11::XMFLOAT3(0.0f, 0.5f, 0.0f), DX11::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 0
			{ DX11::XMFLOAT3(0.5f, -0.5f, 0.0f), DX11::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 1
			{ DX11::XMFLOAT3(-0.5f, -0.5f, 0.0f), DX11::XMFLOAT3(0.0f, 0.0f, 1.0f) },//2
			{ DX11::XMFLOAT3(-0.3f, 0.3f, 0.0f), DX11::XMFLOAT3(0.0f, 1.0f, 0.0f) },//3
			{ DX11::XMFLOAT3(0.3f, 0.3f, 0.0f), DX11::XMFLOAT3(0.0f, 0.0f, 1.0f) },//4
			{ DX11::XMFLOAT3(0.f, -0.8f, 0.0f), DX11::XMFLOAT3(1.0f, 0.0f, 0.0f) },//5
		};
		
		wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC vBufferDesc = {};
		vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vBufferDesc.CPUAccessFlags = 0u;
		vBufferDesc.MiscFlags = 0u;
		vBufferDesc.ByteWidth = sizeof(g_Vertices);
		vBufferDesc.StructureByteStride = sizeof(VertexPosColor);
		D3D11_SUBRESOURCE_DATA vSubresourceData = {};
		vSubresourceData.pSysMem = g_Vertices;
		
		HRESULT hr;
		GFX_THROW_INFO(pDevice->CreateBuffer(&vBufferDesc, &vSubresourceData, &pVertexBuffer));
		
		// Bind Vertex Buffer to pipeline
		UINT const stride = sizeof(VertexPosColor);
		UINT const offset = 0u;
		pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

		// Create Index Buffer
		/*
		WORD g_Indicies[36] = // CUBE
		{
			0, 1, 2, 0, 2, 3,
			4, 6, 5, 4, 7, 6,
			4, 5, 1, 4, 1, 0,
			3, 2, 6, 3, 6, 7,
			1, 5, 6, 1, 6, 2,
			4, 0, 3, 4, 3, 7
		};
		*/
		
		unsigned short g_Indicies[] =
		{
			0,1,2,
			0,2,3,
			0,4,1,
			2,1,5
		};
		
		wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.CPUAccessFlags = 0u;
		indexBufferDesc.MiscFlags = 0u;
		indexBufferDesc.ByteWidth = sizeof(g_Indicies);
		indexBufferDesc.StructureByteStride = sizeof(WORD);
		D3D11_SUBRESOURCE_DATA indexSubresourceData = {};
		indexSubresourceData.pSysMem = g_Indicies;
		GFX_THROW_INFO(pDevice->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &pIndexBuffer));

		//Bind Index Buffer to pipeline
		pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

		// create pixel shader
		wrl::ComPtr<ID3D11PixelShader> pPixelShader;
		wrl::ComPtr<ID3DBlob> pBlob;
		GFX_THROW_INFO(D3DReadFileToBlob(L"../PixelShader.cso", &pBlob));
		GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

		// bind pixel shader
		pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

		// create vertex shader
		wrl::ComPtr<ID3D11VertexShader> pVertexShader;
		
		GFX_THROW_INFO(D3DReadFileToBlob(L"../VertexShader.cso", &pBlob));
		GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

		// bind vertex shader
		pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);


		// input (Vertex) layout 
		wrl::ComPtr<ID3D11InputLayout> pInputLayout;

		const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,offsetof(VertexPosColor, Position), D3D11_INPUT_PER_VERTEX_DATA,0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,offsetof(VertexPosColor, Color), D3D11_INPUT_PER_VERTEX_DATA,0}
		};

		GFX_THROW_INFO(pDevice->CreateInputLayout(
			inputElementDesc, (UINT)std::size(inputElementDesc),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout)
		);

		// bind vertex layout
		pContext->IASetInputLayout(pInputLayout.Get());


		// bind render target
		pContext->OMSetRenderTargets(1u, pRenderTargetView.GetAddressOf(), nullptr);

		// Set Primitive topology to triangle list (groups of 3 vertices)
		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set viewport
		D3D11_VIEWPORT viewport_desc;
		viewport_desc.Width = 800;
		viewport_desc.Height = 600;
		viewport_desc.MinDepth = 0;
		viewport_desc.MaxDepth = 1;
		viewport_desc.TopLeftX = 100;
		viewport_desc.TopLeftY = 100;
		pContext->RSSetViewports(1u, &viewport_desc);

		GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(g_Indicies),0u, 0u));
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
