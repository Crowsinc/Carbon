#pragma once

#include <glm/glm.hpp>

namespace cbn
{

	class Rotatable2D
	{
	private:

		float m_RotationDegrees;

		float wrap_degrees(const float degrees);

	protected:

		virtual void on_rotate();

	public:

		Rotatable2D();

		Rotatable2D(const float degrees);

		void rotate_by(const float degrees);
		
		void rotate_to(const float degrees);
		
		void rotate_by_radians(const float radians);
		
		void rotate_to_radians(const float radians);

		float rotation_degrees() const;

		float rotation_radians() const;

	};

}