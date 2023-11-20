#include "pch.h"
#include "Camera.h"

#include "Input/InputState.h"

using namespace DirectX;

namespace nf::render {
	using namespace input;
	using enum Camera::Type;

	Camera::Camera(Vec3 position)
		: m_type(UI)
		, m_position(position)
		, m_yaw(90.0f), m_pitch(0.0f)
	{}

	void Camera::setType(Type type) {
		m_type = type;

		InputState::captureMouse(m_type == FPS);
	}

	void Camera::update() {
		switch (m_type) {
			case UI:

				break;

			case FPS: {
				int mouseDiffX = 0, mouseDiffY = 0;
				InputState::getMouseDiff(mouseDiffX, mouseDiffY);
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
		if (InputState::isKeyHeld(Code::Shift))
			speed *= 3;

		if (InputState::isKeyHeld(Code::W))
			m_position += getRotation() * speed;
		if (InputState::isKeyHeld(Code::S))
			m_position -= getRotation() * speed;
		if (InputState::isKeyHeld(Code::D)) {
			m_position -= getRotation().cross(Vec3(0.0f, 1.0f, 0.0f)).normalize() * speed;
		}
		if (InputState::isKeyHeld(Code::A)) {
			m_position += getRotation().cross(Vec3(0.0f, 1.0f, 0.0f)).normalize() * speed;
		}

		if (InputState::isKeyPress(Code::E))
			m_type == FPS ? setType(UI) : setType(FPS);
	}

	Vec3 Camera::getPosition() const {
		return m_position;
	}

	Vec3 Camera::getRotation() const {
		Vec3 rot;
		rot.x = std::cos(XMConvertToRadians(m_yaw)) * std::cos(XMConvertToRadians(m_pitch));
		rot.y = std::sin(XMConvertToRadians(m_pitch));
		rot.z = std::sin(XMConvertToRadians(m_yaw)) * std::cos(XMConvertToRadians(m_pitch));
		return rot;
	}

	Camera::~Camera() {

	}
}