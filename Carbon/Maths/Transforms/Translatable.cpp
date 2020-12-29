#include "Translatable.hpp"

#include <glm/gtx/projection.hpp>

#include "../Matrix.hpp"
#include "../Maths.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------
	
	Translatable2D::Translatable2D()
		: Translatable2D(0, 0) {}
	
	//-------------------------------------------------------------------------------------

	Translatable2D::Translatable2D(const glm::vec2& position)
		: Translatable2D(position.x, position.y) {}
	
	//-------------------------------------------------------------------------------------

	Translatable2D::Translatable2D(const float x, const float y)
		: m_Translation(x, y) {}
	
	//-------------------------------------------------------------------------------------

	void Translatable2D::translate_by(const float x, const float y)
	{
		m_Translation.x += x;
		m_Translation.y += y;
	}
	
	//-------------------------------------------------------------------------------------

	void Translatable2D::translate_by(const glm::vec2& translation)
	{
		m_Translation += translation;
	}
	
	//-------------------------------------------------------------------------------------

	void Translatable2D::translate_to(const float x, const float y)
	{
		m_Translation.x = x;
		m_Translation.y = y;
	}
	
	//-------------------------------------------------------------------------------------

	void Translatable2D::translate_to(const glm::vec2& position)
	{
		m_Translation = position;
	}
	
	//-------------------------------------------------------------------------------------

	void Translatable2D::translate_towards(const float x, const float y, const float amount)
	{
		translate_towards({x,y}, amount);
	}
	
	//-------------------------------------------------------------------------------------

	void Translatable2D::translate_towards(const float heading_degrees, const float amount)
	{
		// Get the heading in radians, then use trigonometry to figure out 
	    // how much we need to move in each direction
		const float heading_radians = to_radians(heading_degrees);
		translate_by(amount * sinf(heading_radians), amount * cosf(heading_radians));
	}
	
	//-------------------------------------------------------------------------------------

	void Translatable2D::translate_towards(const glm::vec2& position, const float amount)
	{
		// Project a vector from the current position to the given position
        // then scale it by the amount we want to move, and translate by it
		translate_by(glm::normalize(position - m_Translation) * amount);
	}
	
	//-------------------------------------------------------------------------------------

	const glm::vec2& Translatable2D::translation() const
	{
		return m_Translation;
	}

	//-------------------------------------------------------------------------------------

}