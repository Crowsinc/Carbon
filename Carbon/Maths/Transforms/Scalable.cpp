#include "Scalable.hpp"

#include "../Maths.hpp"
#include "../Matrix.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	Scalable2D::Scalable2D()
		: Scalable2D(1.0f, 1.0f) {}

	//-------------------------------------------------------------------------------------

	Scalable2D::Scalable2D(const float scale)
		: Scalable2D(scale, scale) {}

	//-------------------------------------------------------------------------------------

	Scalable2D::Scalable2D(const glm::vec2& scale)
		: Scalable2D(scale.x, scale.y) {}

	//-------------------------------------------------------------------------------------

	Scalable2D::Scalable2D(const float scale_x, const float scale_y)
		: m_Scale(scale_x, scale_y) {}

	//-------------------------------------------------------------------------------------

	void Scalable2D::scale_by(const float scale)
	{
		return scale_by(scale, scale);
	}

	//-------------------------------------------------------------------------------------

	void Scalable2D::scale_by(const glm::vec2& scaling)
	{
		m_Scale += scaling;
	}

	//-------------------------------------------------------------------------------------

	void Scalable2D::scale_by(const float scale_x, const float scale_y)
	{
		m_Scale.x += scale_x;
		m_Scale.y += scale_y;
	}

	//-------------------------------------------------------------------------------------

	void Scalable2D::scale_to(const float scale)
	{
		return scale_to(scale, scale);
	}

	//-------------------------------------------------------------------------------------

	void Scalable2D::scale_to(const glm::vec2& scaling)
	{
		m_Scale = scaling;
	}

	//-------------------------------------------------------------------------------------

	void Scalable2D::scale_to(const float scale_x, const float scale_y)
	{
		m_Scale.x = scale_x;
		m_Scale.y = scale_y;
	}
	
	//-------------------------------------------------------------------------------------

	const glm::vec2& Scalable2D::scale() const
	{
		return m_Scale;
	}

	//-------------------------------------------------------------------------------------
	
	Scalable1D::Scalable1D()
		: Scalable1D(1.0f) {}
	
	//-------------------------------------------------------------------------------------

	Scalable1D::Scalable1D(const float scale)
		: m_Scale(scale) {}
	
	//-------------------------------------------------------------------------------------

	void Scalable1D::scale_by(const float scale)
	{
		m_Scale += scale;
	}
	
	//-------------------------------------------------------------------------------------

	void Scalable1D::scale_to(const float scale)
	{
		m_Scale = scale;
	}

	//-------------------------------------------------------------------------------------

	float Scalable1D::scale() const
	{
		return m_Scale;
	}
	
	//-------------------------------------------------------------------------------------

}
