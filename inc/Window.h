#pragma once
#include <RenderingPipelineDX11PCH.h>

#include "Graphics.h"


// Inspired by Chili https://wiki.planetchili.net/index.php/Hardware_3D_Series_(C%2B%2B_DirectX_Graphics)
class Window
{
public: 
	class Exception : public SGD3DException
	{
		using SGD3DException::SGD3DException;
	public:
		static std::string TranslateErrorCode(HRESULT hResult) noexcept;
	};

	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hResult) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hResult;
	};

	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};

private:
	// Singleton class
	class WindowClass
	{
	public:
		static LPCWSTR GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		// Removing any copy constructors
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator= (const WindowClass&) = delete;
		static constexpr LPCWSTR wndClassName = L"DirectX 11 Window Class";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	Window(int width, int height, const LPCWSTR name, int cmdShow );
	~Window();
	// Removing any copy constructors
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const LPCWSTR title);
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool CursorEnabled() const noexcept;
	static std::optional<int> ProcessMessages() noexcept;
	Graphics& Gfx();

private: 
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	void ShowCursor() noexcept;
	void HideCursor() noexcept;
	static LRESULT CALLBACK HandleMsgInit(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgContinuous(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

public:
	Keyboard keyboard;
	Mouse mouse;
private:
	bool cursorEnabled = true;
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
	std::vector<BYTE> rawBuffer;
};


