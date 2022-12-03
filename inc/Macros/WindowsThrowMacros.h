#pragma once

#define SGD3DWND_EXCEPT( hResult ) Window::HrException(__LINE__,__FILE__, (hResult))
#define SGD3DWND_LAST_EXCEPT() Window::HrException(__LINE__,__FILE__, GetLastError())
#define SGD3DWND_NOGFX_EXCEPT() Window::NoGfxException(__LINE__,__FILE__)