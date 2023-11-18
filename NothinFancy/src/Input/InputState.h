#pragma once
#include <unordered_map>
#include <atomic>

#include "InputCodes.h"

namespace nf {
	class Window;
}

namespace nf::input {
	class InputState {
	public:
		InputState(const InputState& other) = delete;
		void operator=(const InputState& other) = delete;

		static InputState& get();
		static void initialize(Window* window);

		static bool isKeyHeld(Code key);
		static bool isKeyPress(Code key);

		static void updateState(Code key, bool pressed);
		static void updateMouse();

		static void captureMouse(bool capture);
		static void getMouseDiff(int& x, int& y);
	private:
		InputState();
		void i_initialize(Window* window);

		bool i_keyDown(Code key);
		bool i_keyPress(Code key);

		void i_updateState(Code key, bool pressed);
		void i_updateMouse();

		void i_captureMouse(bool capture);
		void i_getMouseDiff(int& x, int& y);

		~InputState();

		std::unordered_map<Code, std::atomic<bool>> m_state;
		std::unordered_map<Code, std::atomic<bool>> m_pressState;

		Window* m_window;

		bool m_mouseCaptured;
		bool m_mouseCaptureReset;
		int m_mouseDiffX, m_mouseDiffY;
	};
}