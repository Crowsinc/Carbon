#include "Rotatable.hpp"

#include "../Maths.hpp"
#include "../Matrix.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	float Rotatable2D::wrap_degrees(const float degrees)
	{
		constexpr float max_degrees = 360.0f;

		return std::fmodf(degrees, max_degrees);
	}
	
	//-------------------------------------------------------------------------------------

	Rotatable2D::Rotatable2D()
		: Rotatable2D(0) {}
	
	//-------------------------------------------------------------------------------------

	Rotatable2D::Rotatable2D(const float degrees)
		: m_RotationDegrees(0) {}
	
	//-------------------------------------------------------------------------------------

	void Rotatable2D::rotate_by(const float degrees)
	{
		m_RotationDegrees = wrap_degrees(m_RotationDegrees + degrees);
	}
	
	//-------------------------------------------------------------------------------------

	void Rotatable2D::rotate_to(const float degrees)
	{
		m_RotationDegrees = wrap_degrees(degrees);
	}
	
	//-------------------------------------------------------------------------------------

	void Rotatable2D::rotate_by_radians(const float radians)
	{
		rotate_by(to_degrees(radians));
	}
	
	//-------------------------------------------------------------------------------------

	void Rotatable2D::rotate_to_radians(const float radians)
	{
		rotate_to(to_degrees(radians));
	}
	
	//-------------------------------------------------------------------------------------

	float Rotatable2D::rotation_degrees() const
	{
		return m_RotationDegrees;
	}
	
	//-------------------------------------------------------------------------------------

	float Rotatable2D::rotation_radians() const
	{
		return to_radians(m_RotationDegrees);
	}
	
	//-------------------------------------------------------------------------------------

}