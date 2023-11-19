#pragma once
#include "Vector.h"

namespace nf::render {
	class Camera {
	public:
		enum class Type {
			UI,
			FPS
		};

		Camera(Vec3 position);

		void setType(Type type);

		void update();
		Vec3 getPosition() const;
		Vec3 getRotation() const;

		~Camera();
	private:
		Type m_type;

		Vec3 m_position;
		float m_yaw, m_pitch;
	};
}