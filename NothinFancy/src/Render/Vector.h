#pragma once

namespace nf::render {
	struct Vec3 {
		float x, y, z;

		Vec3()
			: x(0.0f), y(0.0f), z(0.0f)
		{}

		Vec3(float val)
			: x(val), y(val), z(val)
		{}

		Vec3(float x, float y, float z)
			: x(x), y(y), z(z)
		{}
	};
}