#include <RenderingPipelineDX11PCH.h>
#include "App.h"

App::App(LPCWSTR windowName, int cmdShow)
	:wnd(1200, 700, windowName, cmdShow)
{
}

int App::Init()
{
	static DWORD previousTime = timeGetTime();
	static float const targetFramerate = 30.0f;
	static float const maxTimeStep = 1.0f / targetFramerate;

	while (true)
	{
		// process all messages  pending, but don't block new messages
		if (const auto ecode = Window::ProcessMessages())
		{
			// if return optional has value, means we're quiting so return exit code
			return *ecode;
		}

		// execute game logic
		DWORD currentTime = timeGetTime();
		float deltaTime = (currentTime - previousTime) / 1000.0f;
		previousTime = currentTime;
		// Cap the delta time to the max time step (useful if your 
		// debugging and you don't want the deltaTime value to explode.
		deltaTime = std::min<float>(deltaTime, maxTimeStep);

		DoFrame();
		//GatherInput(deltaTime)
		//RenderFrame(deltaTime)
	}

}

void App::DoFrame()
{	
	const float col = sin(timeGetTime()/1000.f) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(col, col, 1.0f);
	wnd.Gfx().DrawTestTriangle(
		timeGetTime() / 1000.f,
		0.0f,
		0.0f
	);

	wnd.Gfx().DrawTestTriangle(
		timeGetTime() / 1000.f,
		wnd.mouse.GetPosX() / 600.0f - 1.0f,
		-wnd.mouse.GetPosY() / 350.0f + 1.0f
	);
	wnd.Gfx().EndFrame();
}
