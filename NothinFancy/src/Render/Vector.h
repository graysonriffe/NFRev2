#pragma once

namespace nf::render {
	struct Vec2 {
		float x, y;

		Vec2()
			: x(0.0f), y(0.0f)
		{}

		Vec2(float val)
			: x(val), y(val)
		{}

		Vec2(float x, float y)
			: x(x), y(y)
		{}

		Vec2 operator+(const Vec2& rhs) {
			return Vec2(x + rhs.x, y + rhs.y);
		}

		Vec2 operator-(const Vec2& rhs) {
			return Vec2(x - rhs.x, y - rhs.y);
		}

		Vec2 operator*(const float coeff) {
			return Vec2(x * coeff, y * coeff);
		}

		Vec2 operator/(const Vec2& rhs) const {
			return Vec2(x / rhs.x, y / rhs.y);
		}

		Vec2& operator+=(const Vec2& rhs) {
			this->x += rhs.x, this->y += rhs.y;
			return *this;
		}

		Vec2& operator-=(const Vec2& rhs) {
			this->x -= rhs.x, this->y -= rhs.y;
			return *this;
		}

		Vec2& operator*=(const Vec2& rhs) {
			this->x *= rhs.x, this->y *= rhs.y;
			return *this;
		}

		Vec2& operator/=(const Vec2& rhs) {
			this->x /= rhs.x, this->y /= rhs.y;
			return *this;
		}

		Vec2& cross(const Vec2& rhs) {
			float tempX = x, tempY = y;
			x = tempX * rhs.y - tempY * rhs.x;
			y = tempX * rhs.y - tempY * rhs.x;
			return *this;
		}

		float length() {
			return std::sqrtf(x * x + y * y);
		}

		Vec2& normalize() {
			float lInv = 1.0f / length();
			x *= lInv, y *= lInv;
			return *this;
		}
	};

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

		Vec3 operator+(const Vec3& rhs) {
			return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
		}

		Vec3 operator-(const Vec3& rhs) {
			return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
		}

		Vec3 operator*(const float coeff) {
			return Vec3(x * coeff, y * coeff, z * coeff);
		}

		Vec3 operator/(const Vec3& rhs) const {
			return Vec3(x / rhs.x, y / rhs.y, z / rhs.z);
		}

		Vec3& operator+=(const Vec3& rhs) {
			this->x += rhs.x, this->y += rhs.y, this->z += rhs.z;
			return *this;
		}

		Vec3& operator-=(const Vec3& rhs) {
			this->x -= rhs.x, this->y -= rhs.y, this->z -= rhs.z;
			return *this;
		}

		Vec3& operator*=(const Vec3& rhs) {
			this->x *= rhs.x, this->y *= rhs.y, this->z *= rhs.z;
			return *this;
		}

		Vec3& operator/=(const Vec3& rhs) {
			this->x /= rhs.x, this->y /= rhs.y, this->z /= rhs.z;
			return *this;
		}

		Vec3& cross(const Vec3& rhs) {
			float tempX = x, tempY = y, tempZ = z;
			x = tempY * rhs.z - tempZ * rhs.y;
			y = tempZ * rhs.x - tempX * rhs.z;
			z = tempX * rhs.y - tempY * rhs.x;
			return *this;
		}

		float length() {
			return std::sqrtf(x * x + y * y + z * z);
		}

		Vec3& normalize() {
			float lInv = 1.0f / length();
			x *= lInv, y *= lInv, z *= lInv;
			return *this;
		}
	};
}