#include "pch.h"
#include "Camera.h"

#include "Input/InputState.h"

namespace nf::render {
	Camera::Camera(Vec3 position)
		: m_type(Type::UI)
		, m_position(position)
		, m_yaw(90.0f), m_pitch(0.0f)
	{}

	void Camera::setType(Type type) {
		m_type = type;

		input::InputState::captureMouse(m_type == Type::FPS);
	}

	void Camera::update() {
		using enum Type;

		switch (m_type) {
			case UI:

				break;

			case FPS: {
				int mouseDiffX = 0, mouseDiffY = 0;
				input::InputState::getMouseDiff(mouseDiffX, mouseDiffY);
				m_yaw -= static_cast<float>(mouseDiffX) * 0.1f;
				m_pitch -= static_cast<float>(mouseDiffY) * 0.1f;

				if (m_pitch > 89.0f)
					m_pitch = 89.0f;
				if (m_pitch < -89.0f)
					m_pitch = -89.0f;

				break;
			}
		}

		float speed = 0.1f;
		if (input::InputState::isKeyHeld(input::Code::Shift))
			speed *= 3;

		if (input::InputState::isKeyHeld(input::Code::W))
			m_position += getRotation() * speed;
		if (input::InputState::isKeyHeld(input::Code::S))
			m_position -= getRotation() * speed;
		if (input::InputState::isKeyHeld(input::Code::D)) {
			m_position -= getRotation().cross(Vec3(0.0f, 1.0f, 0.0f)).normalize() * speed;
		}
		if (input::InputState::isKeyHeld(input::Code::A)) {
			m_position += getRotation().cross(Vec3(0.0f, 1.0f, 0.0f)).normalize() * speed;
		}

		if (input::InputState::isKeyPress(input::Code::E))
			m_type == Type::FPS ? setType(Type::UI) : setType(Type::FPS);
	}

	Vec3 Camera::getPosition() const {
		return m_position;
	}

	Vec3 Camera::getRotation() const {
		Vec3 rot;
		rot.x = std::cos(DirectX::XMConvertToRadians(m_yaw)) * std::cos(DirectX::XMConvertToRadians(m_pitch));
		rot.y = std::sin(DirectX::XMConvertToRadians(m_pitch));
		rot.z = std::sin(DirectX::XMConvertToRadians(m_yaw)) * std::cos(DirectX::XMConvertToRadians(m_pitch));
		return rot;
	}

	Camera::~Camera() {

	}
}