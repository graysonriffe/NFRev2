#pragma once
#include "Vector.h"

namespace nf::render {
	class Light {
	public:
		enum class Type {
			Point
		};

		Light(Type type, Vec3 position);

		Type getType() const;

		void setPosition(Vec3 position);
		Vec3 getPosition() const;

		void setColor(Vec3 color);
		Vec3 getColor() const;

		~Light();
	private:
		Type m_type;

		Vec3 m_position;
		Vec3 m_color;
	};
}