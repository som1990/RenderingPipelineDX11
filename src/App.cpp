#include <RenderingPipelineDX11PCH.h>
#include "App.h"


App::App(LPCWSTR windowName, int cmdShow)
	:wnd(1200, 700, windowName, cmdShow)
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	for (auto i = 0; i < 80; i++)
	{
		boxes.push_back(std::make_unique<Box>(
			wnd.Gfx(), rng, adist,
			ddist, odist, rdist
			));
	}
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
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



		DoFrame();

	}

}

App::~App()
{}

void App::DoFrame()
{	
	auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
	for (auto& b : boxes)
	{
		b->Update(dt);
		b->Draw(wnd.Gfx());
	}
	wnd.Gfx().EndFrame();
}
