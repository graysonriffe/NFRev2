#pragma once
#include "Vector.h"

namespace nf::render {
	class Camera {
	public:
		Camera(Vec3 position);

		void update();
		Vec3 getPosition() const;
		Vec3 getRotation() const;

		~Camera();
	private:
		Vec3 m_position;

		float m_yaw, m_pitch;
	};
}