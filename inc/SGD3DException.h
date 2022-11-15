#pragma once
#include <exception>
#include <string>


// Taken from ChilliException https://github.com/planetchili/hw3d/blob/master/hw3d/ChiliException.h
class SGD3DException : public std::exception
{
public:
	SGD3DException(int line, const char* file) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;

private:
	int line;
	std::string file;

protected:
	mutable std::string whatBuffer;
};