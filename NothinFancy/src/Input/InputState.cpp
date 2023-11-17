#include "pch.h"
#include "InputState.h"

namespace nf::input {
	InputState::InputState() {

	}

	InputState& InputState::get() {
		static InputState instance;
		return instance;
	}

	bool InputState::isKeyHeld(Code key) {
		return get().i_keyDown(key);
	}

	bool InputState::isKeyPress(Code key) {
		return get().i_keyPress(key);
	}

	//Implementation

	bool InputState::i_keyDown(Code key) {
		return m_state[key];
	}

	bool InputState::i_keyPress(Code key) {
		bool result = m_pressState[key];
		m_pressState[key] = false;
		return result;
	}

	void InputState::updateState(Code key, bool pressed) {
		m_state[key] = pressed, m_pressState[key] = pressed;
	}

	InputState::~InputState() {

	}
}