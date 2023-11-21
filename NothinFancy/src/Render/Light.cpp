#include "pch.h"
#include "Light.h"

namespace nf::render {
	using enum Light::Type;

	Light::Light(Type type, Vec3 position)
		: m_type(type)
		, m_position(position)
		, m_color(Vec3(1.0f, 1.0f, 1.0f))
	{}

	Light::Type Light::getType() const {
		return m_type;
	}

	void Light::setPosition(Vec3 position) {
		m_position = position;
	}

	Vec3 Light::getPosition() const {
		return m_position;
	}

	void Light::setColor(Vec3 color) {
		m_color = color;
	}

	Vec3 Light::getColor() const {
		return m_color;
	}

	Light::~Light() {

	}
}