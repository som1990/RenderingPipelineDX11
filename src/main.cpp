#include "RenderingPipelineDX11PCH.h"
#include "Window.h"
#include "App.h"

namespace DX11 = DirectX;

// DIRECTX INITIALIZATION
LONG const g_WindowWidth = 1200;
LONG const g_WindowHeight = 720;
LPCWSTR g_WindowClassName = L"DirectXWindowClass";
LPCWSTR g_WindowName = L"DirectX 11 Template";
HWND g_WindowHandle = 0;

BOOL const g_EnableVSync = TRUE;


// Direct3D device and swap chain

ID3D11Device *g_d3dDevice = nullptr;

ID3D11DeviceContext *g_d3dDeviceContext = nullptr;

IDXGISwapChain *g_d3dSwapChain = nullptr;

// Render target view for the back buffer of the swap chain

ID3D11RenderTargetView *g_d3dRenderTargetView = nullptr;

// Depth/stencil view for use as a depth buffer.

ID3D11DepthStencilView *g_d3dDepthStencilView = nullptr;

// A texture to associate to the depth stencil view

ID3D11Texture2D *g_d3dDepthStencilBuffer = nullptr;

// Define the functionality of the depth/stencil stages.

ID3D11DepthStencilState *g_d3dDepthStencilState = nullptr;

// Define the functionality of the rasterizer stage

ID3D11RasterizerState *g_d3dRasterizerState = nullptr;

D3D11_VIEWPORT g_Viewport = { 0 };

// PROJECT SPECIFIC Variables

// Vertex buffer data

ID3D11InputLayout *g_d3dInputLayout = nullptr;

ID3D11Buffer *g_d3dVertexBuffer = nullptr;

ID3D11Buffer *g_d3dIndexBuffer = nullptr;

// Shader data

ID3D11VertexShader *g_d3dVertexShader = nullptr;

ID3D11PixelShader *g_d3dPixelShader = nullptr;

// Shader Resources

enum ConstantBuffer
{
	CB_Application,
	CB_Frame,
	CB_Object,
	NumConstantBuffers
};

ID3D11Buffer* g_d3dConstantBuffers[NumConstantBuffers];

// Demo Parameters

DX11::XMMATRIX g_WorldMatrix;

DX11::XMMATRIX g_ViewMatrix;

DX11::XMMATRIX g_ProjectionMatrix;

// Vertex Data for a colored cube

struct VertexPosColor
{
	DX11::XMFLOAT3 Position;
	DX11::XMFLOAT3 Color;
};

VertexPosColor g_Vertices[8] =
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

WORD g_Indicies[36] =
{
	0, 1, 2, 0, 2, 3,
	4, 6, 5, 4, 7, 6,
	4, 5, 1, 4, 1, 0,
	3, 2, 6, 3, 6, 7,
	1, 5, 6, 1, 6, 2,
	4, 0, 3, 4, 3, 7
};

// Forward declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

template< class ShaderClass >
ShaderClass* LoadShader(const std::wstring& fileName, const std::string& entryPoint, const std::string& profile);

bool LoadContent();
void UnloadContent();

void Update(float deltaTime);
void Render();
void Cleanup();


/*
	Initialize the application window
*/

int InitApplication(HINSTANCE hInstance, int cmdShow)
{
	
	return 0;
}

DXGI_RATIONAL QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync)
{
	DXGI_RATIONAL refreshRate = { 0, 1 };

	if (vsync)
	{
		IDXGIFactory *factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;
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

			throw new std::exception("Failed to create DXGIFactory.");
		}

		gfxFactoryResult = factory->EnumAdapters(0, &adapter);
		if (FAILED(gfxFactoryResult))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapters."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to enumerate adapters.");
		}
		gfxFactoryResult = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(gfxFactoryResult))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapter outputs."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to enumerate adapter outputs.");
		}

		UINT numDisplayModes;
		gfxFactoryResult = adapterOutput->GetDisplayModeList(
				DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr
		);

		if (FAILED(gfxFactoryResult))
		{
			MessageBox(0,
				TEXT("Failed to query display mode list."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to query display mode list.");
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

			throw new std::exception("Failed to query display mode list.");
		}

		// Store the refresh rate of the monitor that matches the width and height of the requested screen.
		for (UINT i = 0; i < numDisplayModes; ++i)
		{
			if (displayModeList[i].Width == screenWidth && displayModeList[i].Height == screenHeight)
			{
				refreshRate = displayModeList[i].RefreshRate;
			}
		}

		delete[] displayModeList;
		SafeRelease(adapterOutput);
		SafeRelease(adapter);
		SafeRelease(factory);
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

int InitDirectX(HINSTANCE hInstance, BOOL vSync)
{
	// A window handle needs to already be initialized
	assert(g_WindowHandle != 0);

	RECT clientRect;
	GetClientRect(g_WindowHandle, &clientRect);

	// compute the exact client dimensions. This is used to initialize the render targets
	unsigned int clientWidth = clientRect.right - clientRect.left;
	unsigned int clientHeight = clientRect.bottom - clientRect.top;


	// 1) Create Device and swap chain

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = clientWidth;
	swapChainDesc.BufferDesc.Height = clientHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate = QueryRefreshRate(clientWidth, clientHeight, vSync);
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = g_WindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = TRUE;

	UINT createDeviceFlags = 0;
#if _DEBUG
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

	HRESULT directX11InitResult = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevels, _countof(featureLevels), 
		D3D11_SDK_VERSION, &swapChainDesc, &g_d3dSwapChain, &g_d3dDevice, &featureLevel, &g_d3dDeviceContext
	);

	if (FAILED(directX11InitResult))
	{
		return -1;
	}

	// 2) Create a render target view of the swap chain’s back buffer
	ID3D11Texture2D* backBuffer;
	directX11InitResult = g_d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

	if (FAILED(directX11InitResult))
	{
		return -1;
	}

	directX11InitResult = g_d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &g_d3dRenderTargetView);
	if (FAILED(directX11InitResult))
	{
		return -1;
	}

	SafeRelease(backBuffer);

	// 3) Create a texture for the depth-stencil buffer
	
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

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

	directX11InitResult = g_d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &g_d3dDepthStencilBuffer);

	if (FAILED(directX11InitResult))
	{
		return -1;
	}

	// 4) Create a depth-stencil view from the depth-stencil buffer

	directX11InitResult = g_d3dDevice->CreateDepthStencilView(g_d3dDepthStencilBuffer, nullptr, &g_d3dDepthStencilView);

	if (FAILED(directX11InitResult))
	{
		return -1;
	}

	// 5) Create a depth-stencil state object that defines the behaviour of the output merger stage,

	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilStateDesc.DepthEnable = TRUE;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilStateDesc.StencilEnable = FALSE;

	directX11InitResult = g_d3dDevice->CreateDepthStencilState(&depthStencilStateDesc, &g_d3dDepthStencilState);

	// 6) Create a rasterizer state object that defines the behaviour of the rasterizer stage.

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;

	directX11InitResult = g_d3dDevice->CreateRasterizerState(&rasterizerDesc, &g_d3dRasterizerState);

	if (FAILED(directX11InitResult))
	{
		return -1;
	}

	// Initialize the viewport the occupy the entire client area
	g_Viewport.Width = (float)(clientWidth);
	g_Viewport.Height = (float)(clientHeight);
	g_Viewport.TopLeftX = 0.f;
	g_Viewport.TopLeftY = 0.f;
	g_Viewport.MinDepth = 0.f;
	g_Viewport.MaxDepth = 1.f;

	return 0;


}
/*
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hDC;

	switch (message)
	{
	case WM_PAINT:
	{
			hDC = BeginPaint(hwnd, &paintStruct);
			EndPaint(hwnd, &paintStruct);
			break;
	}
	case WM_DESTROY:
	{
			PostQuitMessage(25);
			break;
	}
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(26);
		}
		break;
	}
	default:
	{
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	}// switch(message)

	return 0;
}
*/

int Run()
{
	MSG msg = { 0 };

	static DWORD previousTime = timeGetTime();
	static float const targetFramerate = 30.0f;
	static float const maxTimeStep = 1.0f / targetFramerate;

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			DWORD currentTime = timeGetTime();
			float deltaTime = (currentTime - previousTime) / 1000.0f;
			previousTime = currentTime;
			// Cap the delta time to the max time step (useful if your 
			// debugging and you don't want the deltaTime value to explode.
			deltaTime = std::min<float>(deltaTime, maxTimeStep);

			// Update (deltaTime);
			// Render();

		}
	}

	return int(msg.wParam);

}

// ENTRY POINT
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);
	
	// Check for DirectX Math Library Support
	if (!DX11::XMVerifyCPUSupport())
	{
		MessageBox(nullptr, TEXT("Failed to verify DirectX Math library support."), TEXT("Error"), MB_OK);
		return -1;
	}
	try
	{
		return App(g_WindowName, cmdShow).Init();
	}
	catch (const SGD3DException& err)
	{
		wchar_t* whatStr = new wchar_t[4096];
		MultiByteToWideChar(CP_ACP, 0, err.what(), -1, whatStr, 4096);
		wchar_t* typeStr = new wchar_t[4096];
		MultiByteToWideChar(CP_ACP, 0, err.GetType(), -1, typeStr, 4096);
		MessageBox(nullptr, whatStr, typeStr, MB_OK | MB_ICONEXCLAMATION);
		
	}
	catch (const std::exception& err)
	{
		wchar_t* whatStr = new wchar_t[4096];
		MultiByteToWideChar(CP_ACP, 0, err.what(), -1, whatStr, 4096);
		MessageBox(nullptr, whatStr, TEXT("Standard Exception") , MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, TEXT("No details available"), TEXT("Unknown Exception"), MB_OK | MB_ICONEXCLAMATION);
	}
	//int returnCode = Run();
	//return returnCode
	return -1;
}