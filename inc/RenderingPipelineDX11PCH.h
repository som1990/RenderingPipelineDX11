#pragma once

// System includes
// target Windows 7 or later
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>
/*
#ifndef DISABLE_WINDOWS_MISCs
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NODRAWTEXT
#define NOKERNEL
//#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#endif

#define NOMINMAX
//#define STRICT
*/
#include <windows.h>
#include <tchar.h>
#include <stringapiset.h>
#pragma warning(push)
#pragma warning(disable:4265)
#include <wrl.h>
#pragma warning(pop)

// DirectX includes

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <dxgidebug.h>
#include "dxerr.h"

// STL includes

#include <bitset>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <queue>
#include <vector>


// Link library dependencies

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")

// Safely release a COM object

template<typename T>

inline void SafeRelease(T& ptr)
{
	if (ptr != NULL)
	{
		ptr->Release();
		ptr = NULL;
	}

}

// Project Includes
#include "SGD3DException.h"
#include "Macros/WindowsThrowMacros.h"
#include "Macros/GraphicsThrowMacros.h"
#include "ChiliTimer.h"

//Inputs Classes
#include "Keyboard.h"
#include "Mouse.h"