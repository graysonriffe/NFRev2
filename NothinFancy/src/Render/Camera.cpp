#include "pch.h"
#include "Camera.h"

#include "Input/InputState.h"

namespace nf::render {
	Camera::Camera(Vec3 position)
		: m_position(position)
	{}

	void Camera::update() {
		if (input::InputState::isKeyHeld(input::Code::W))
			m_position.z += 0.1f;
		if (input::InputState::isKeyHeld(input::Code::S))
			m_position.z -= 0.1f;
		if (input::InputState::isKeyHeld(input::Code::D))
			m_position.x += 0.1f;
		if (input::InputState::isKeyHeld(input::Code::A))
			m_position.x -= 0.1f;
	}

	Vec3 Camera::getPosition() const {
		return m_position;
	}

	Camera::~Camera() {

	}
}