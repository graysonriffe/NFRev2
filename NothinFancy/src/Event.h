#pragma once
#include "Input.h"

namespace nf {
	struct Event {
		enum class Type {
			WindowClose,
			KeyPress, KeyRelease,
			MousePress, MouseRelease, MouseScroll
		};

		virtual const Type getType() const = 0;

		virtual ~Event() = default;
	};

	struct WindowCloseEvent : public Event {
		virtual const Type getType() const override { return Type::WindowClose; }
	};

	struct KeyEvent : public Event {
		input::Key key;

	protected:
		KeyEvent(input::Key key)
			: key(key)
		{}
	};

	struct KeyPressEvent : public KeyEvent {
		KeyPressEvent(input::Key key)
			: KeyEvent(key)
		{}

		virtual const Type getType() const override { return Type::KeyPress; }
	};

	struct KeyReleaseEvent : public KeyEvent {
		KeyReleaseEvent(input::Key key)
			: KeyEvent(key)
		{}

		virtual const Type getType() const override { return Type::KeyRelease; }
	};

	struct MouseEvent : public Event {
		input::Mouse button;

	protected:
		MouseEvent(input::Mouse button)
			: button(button)
		{}
	};

	struct MousePressEvent : public MouseEvent {
		MousePressEvent(input::Mouse button)
			: MouseEvent(button)
		{}

		virtual const Type getType() const override { return Type::MousePress; }
	};

	struct MouseReleaseEvent : public MouseEvent {
		MouseReleaseEvent(input::Mouse button)
			: MouseEvent(button)
		{}

		virtual const Type getType() const override { return Type::MouseRelease; }
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