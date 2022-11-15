#pragma once
#include <RenderingPipelineDX11PCH.h>

class Keyboard
{
	friend class Window;
public:
	// Event is a generic name and creating a subclass allows us to encapsulate Event inside of Keyboard.
	// This tells us whenever we create a Keyboard::Event we are refering to our Keyboard class.
	class Event
	{
	public:
		// variables
		enum class Type
		{
			Press,
			Release
		};
	private: 
		// variables
		Type type;
		unsigned char code;
	public:
		// functions
		Event(Type type, unsigned char code) noexcept
			: type(type), code(code)
		{}
		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}
		unsigned char GetCode() const noexcept
		{
			return code;
		}
	};
public:
	// Removing any copy constructors
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	// key event stuff
	bool KeyIsPressed(unsigned char keycode) const noexcept;
	std::optional<Event> ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void ClearKey() noexcept;
	// char event
	std::optional<char> ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void ClearChar() noexcept;
	void Clear() noexcept;
	// autorepeat control
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;

private:
	// functions
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;

private:
	// variables
	static constexpr unsigned int nKeys = 256u; //  a single 32bytes allocated for boolean states of all possible keys.
	static constexpr unsigned int bufferSize = 16u;
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keyStates;
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;

};


