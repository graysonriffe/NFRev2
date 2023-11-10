#pragma once
#include "Input.h"

namespace nf {
	struct Event {
		enum class Type {
			WindowClose,
			KeyPress, KeyRelease,
			MouseMove, MousePress, MouseRelease, MouseScroll
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
}