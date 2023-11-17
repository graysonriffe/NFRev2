#pragma once
#include "InputCodes.h"

namespace nf::input {
	struct Event {
		enum class Type {
			WindowClose,
			KeyPress, KeyRelease,
			MouseScroll
		};

		virtual const Type getType() const = 0;

		virtual ~Event() = default;
	};

	struct WindowCloseEvent : public Event {
		virtual const Type getType() const override { return Type::WindowClose; }
	};

	struct KeyEvent : public Event {
		input::Code key;

	protected:
		KeyEvent(input::Code key)
			: key(key)
		{}
	};

	struct KeyPressEvent : public KeyEvent {
		KeyPressEvent(input::Code key)
			: KeyEvent(key)
		{}

		virtual const Type getType() const override { return Type::KeyPress; }
	};

	struct KeyReleaseEvent : public KeyEvent {
		KeyReleaseEvent(input::Code key)
			: KeyEvent(key)
		{}

		virtual const Type getType() const override { return Type::KeyRelease; }
	};

	struct MouseScrollEvent : Event {
		enum class Direction {
			Up, Down
		};

		Direction direction;

		MouseScrollEvent(Direction direction)
			: direction(direction)
		{}

		virtual const Type getType() const override { return Type::MouseScroll; }
	};
}