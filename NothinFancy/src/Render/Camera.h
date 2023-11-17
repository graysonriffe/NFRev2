#pragma once
#include "Vector.h"

namespace nf::render {
	class Camera {
	public:
		Camera(Vec3 position);

		void update();
		Vec3 getPosition() const;

		~Camera();
	private:
		Vec3 m_position;
	};
}