#pragma once
#include <RenderingPipelineDX11PCH.h>
#include "Window.h"
#include "Box.h"


class App
{
public:
	App(LPCWSTR windowName, int cmdShow);
	// master frame/message loop
	int Init();
	~App();
private:
	void DoFrame();
private:
	Window wnd;
	ChiliTimer timer;
	std::vector<std::unique_ptr<class Box>> boxes;
};