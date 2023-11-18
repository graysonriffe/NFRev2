#include "pch.h"
#include "InputState.h"

#include <Windows.h>

#include "Window.h"
#include "nf/Utility/Util.h"

namespace nf::input {
	InputState& InputState::get() {
		static InputState instance;
		return instance;
	}

	void InputState::initialize(Window* window) {
		get().i_initialize(window);
	}

	bool InputState::isKeyHeld(Code key) {
		return get().i_keyDown(key);
	}

	bool InputState::isKeyPress(Code key) {
		return get().i_keyPress(key);
	}

	void InputState::updateState(Code key, bool pressed) {
		get().i_updateState(key, pressed);
	}

	void InputState::updateMouse() {
		get().i_updateMouse();
	}

	void InputState::captureMouse(bool capture) {
		get().i_captureMouse(capture);
	}

	void InputState::getMouseDiff(int& x, int& y) {
		get().i_getMouseDiff(x, y);
	}

	//Implementation

	InputState::InputState()
		: m_window(nullptr)
		, m_mouseCaptured(false)
		, m_mouseCaptureReset(false)
		, m_mouseDiffX(0), m_mouseDiffY(0)
	{}

	void InputState::i_initialize(Window* window) {
		m_window = window;
	}

	bool InputState::i_keyDown(Code key) {
		return m_state[key];
	}

	bool InputState::i_keyPress(Code key) {
		bool result = m_pressState[key];
		m_pressState[key] = false;
		return result;
	}

	void InputState::i_updateState(Code key, bool pressed) {
		m_state[key] = pressed, m_pressState[key] = pressed;
	}

	void InputState::i_updateMouse() {
		if (!m_mouseCaptured || GetForegroundWindow() != m_window->getHandle())
			return;

		HWND wnd = m_window->getHandle();
		POINT mouse = {};
		GetCursorPos(&mouse);
		ScreenToClient(wnd, &mouse);

		RECT client = {};
		GetClientRect(wnd, &client);
		unsigned int middleX = (client.right - client.left) / 2, middleY = (client.bottom - client.top) / 2;

		if (!m_mouseCaptureReset)
			m_mouseDiffX += mouse.x - middleX, m_mouseDiffY += mouse.y - middleY;
		else
			m_mouseCaptureReset = false;

		mouse.x = middleX, mouse.y = middleY;
		ClientToScreen(wnd, &mouse);
		SetCursorPos(mouse.x, mouse.y);
	}

	void InputState::i_captureMouse(bool capture) {
		m_mouseCaptured = capture;
		if (m_mouseCaptured)
			m_mouseCaptureReset = true;
		m_window->hideCursor(m_mouseCaptured);
	}

	void InputState::i_getMouseDiff(int& x, int& y) {
		x = m_mouseDiffX, m_mouseDiffX = 0;
		y = m_mouseDiffY, m_mouseDiffY = 0;
	}

	InputState::~InputState() {

	}
}