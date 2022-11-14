#include "Line.hpp"

namespace cbn
{
	
	//-------------------------------------------------------------------------------------

	Ray::Ray(const glm::vec2 origin, const glm::vec2 towards)
		: m_Origin(origin),
		  m_Direction(glm::normalize(towards))
	{}

	//-------------------------------------------------------------------------------------

	Ray::Ray(const glm::vec2 origin, const float rotation_degrees)
		: m_Origin(origin),
		  m_Direction(
			  glm::cos(to_radians(rotation_degrees)), 
			  glm::sin(to_radians(rotation_degrees))
		  )
	{}

	//-------------------------------------------------------------------------------------

	void Ray::point_towards(const glm::vec2 towards)
	{
		m_Direction = glm::normalize(towards);
	}

	//-------------------------------------------------------------------------------------

	const glm::vec2& Ray::direction() const
	{
		return m_Direction;
	}

	//-------------------------------------------------------------------------------------

	const glm::vec2& Ray::normal() const
	{
		// 90 degree counter-clockwise rotation. 
		return {-m_Direction.y, m_Direction.x};
	}

	//-------------------------------------------------------------------------------------

	const glm::vec2& Ray::origin() const
	{
		return m_Origin;
	}
	//-------------------------------------------------------------------------------------

	Line::Line()
		: Line({0,0}, {0,0})
	{}

	//-------------------------------------------------------------------------------------

	Line::Line(const glm::vec2 p1, const glm::vec2 p2)
		: m_P1(p1), 
		  m_P2(p2)
	{}

	//-------------------------------------------------------------------------------------

	void Line::reshape(const glm::vec2 p1, const glm::vec2 p2)
	{
		m_P1 = p1;
		m_P2 = p2;
	}
	
	//-------------------------------------------------------------------------------------

	std::tuple<glm::vec2, glm::vec2> Line::points() const
	{
		return std::make_tuple(m_P1, m_P2);
	}

	//-------------------------------------------------------------------------------------

	float Line::gradient() const
	{
		return (m_P1.y - m_P2.y)/(m_P1.x - m_P2.x);
	}

	//-------------------------------------------------------------------------------------

	Segment::Segment()
		: Segment({0,0}, {0,0})
	{}

	//-------------------------------------------------------------------------------------

	Segment::Segment(const glm::vec2 p1, const glm::vec2 p2)
		: m_P1(p1),
		  m_P2(p2)
	{}

	//-------------------------------------------------------------------------------------

	void Segment::reshape(const glm::vec2 p1, const glm::vec2 p2)
	{
		m_P1 = p1;
		m_P2 = p2;
	}

	//-------------------------------------------------------------------------------------

	std::tuple<glm::vec2, glm::vec2> Segment::points() const
	{
		return std::make_tuple(m_P1, m_P2);
	}
	
	//-------------------------------------------------------------------------------------

	glm::vec2 Segment::normal() const
	{
		return Ray(m_P1, m_P2-m_P1).normal();
	}
	
	//-------------------------------------------------------------------------------------

}