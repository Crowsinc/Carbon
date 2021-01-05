#pragma once

#include <glm/glm.hpp>

namespace cbn
{

	class Scalable2D
	{
	private:

		glm::vec2 m_Scale;

	public:

		Scalable2D();

		Scalable2D(const float scale);

		Scalable2D(const glm::vec2& scale);

		Scalable2D(const float scale_x, const float scale_y);

		void scale_by(const float scale);
		
		void scale_by(const glm::vec2& scaling);
		
		void scale_by(const float scale_x, const float scale_y);
		
		void scale_to(const float scale);
		
		void scale_to(const glm::vec2& scaling);
		
		void scale_to(const float scale_x, const float scale_y);

		const glm::vec2& scale() const;

	};

	class Scalable1D
	{
	private:

		float m_Scale;

	public:

		Scalable1D();

		Scalable1D(const float scale);

		void scale_by(float scale);
		
		void scale_to(float scale);

		float scale() const;

	};

}