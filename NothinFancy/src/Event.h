#pragma once

namespace nf {
	class Event {
	public:
		enum class Type {
			WindowClose,
			KeyPress, KeyRelease,
			MouseMove, MousePress, MouseRelease, MouseScroll
		};

		virtual const Type getType() const = 0;

		virtual ~Event() = default;
	};

	class WindowCloseEvent : public Event {
	public:
		virtual const Type getType() const override { return Type::WindowClose; }
	};
}