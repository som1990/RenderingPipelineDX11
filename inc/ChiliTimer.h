#pragma once
#include <RenderingPipelineDX11PCH.h>
#include <chrono>
/*Shamelessly taken from Chilli hw3D tutorial https://github.com/planetchili/hw3d/tree/master/hw3d

*/
class ChiliTimer
{

public:
	ChiliTimer() noexcept;
	float Mark() noexcept;
	float Peek() const noexcept;
private:
	std::chrono::steady_clock::time_point last;
};