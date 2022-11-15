#include "RenderingPipelineDX11PCH.h"
#include "SGD3DException.h"


SGD3DException::SGD3DException(int line, const char* file) noexcept
	: line(line), file(file)
{
}

const char* SGD3DException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
			<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* SGD3DException::GetType() const noexcept
{
	return "SGD3DException";
}

int SGD3DException::GetLine() const noexcept
{
	return line;
}

const std::string& SGD3DException::GetFile() const noexcept
{
	return file;
}

std::string SGD3DException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line]" << line;
	return oss.str();
}