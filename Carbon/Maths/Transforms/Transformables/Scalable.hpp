#pragma once

#include <glm/glm.hpp>

namespace cbn
{

	class Scalable2D
	{
		template<typename T1, typename T2, typename T3> friend class Transformable;
	private:

		glm::vec2 m_Scale;

		virtual void on_scale();

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
		template<typename T1, typename T2, typename T3> friend class Transformable;
	private:

		float m_Scale;

		virtual void on_scale();

	public:

		Scalable1D();

		Scalable1D(const float scale);

		void scale_by(float scale);
		
		void scale_to(float scale);

		float scale() const;

	};

}