#pragma once
#include <RenderingPipelineDX11PCH.h>
#include "Window.h"
class App
{
public:
	App(LPCWSTR windowName, int cmdShow);
	// master frame/message loop
	int Init();
private:
	void DoFrame();
private:
	Window wnd;
};