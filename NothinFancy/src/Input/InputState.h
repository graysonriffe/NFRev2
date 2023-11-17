#pragma once
#include <unordered_map>
#include <atomic>

#include "InputCodes.h"

namespace nf {
	class Application;
}

namespace nf::input {
	class InputState {
	public:
		InputState(const InputState& other) = delete;
		void operator=(const InputState& other) = delete;

		static InputState& get();

		static bool isKeyHeld(Code key);
		static bool isKeyPress(Code key);
	private:
		InputState();
		bool i_keyDown(Code key);
		bool i_keyPress(Code key);

		friend class Application;

		void updateState(Code key, bool pressed);

		~InputState();

		std::unordered_map<Code, std::atomic<bool>> m_state;
		std::unordered_map<Code, std::atomic<bool>> m_pressState;
	};
}