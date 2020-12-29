#include "Maths.hpp"

#include <algorithm>
#include <glm/gtx/vector_query.hpp>

namespace cbn
{

	//-------------------------------------------------------------------------------------

	int next_power(int value, int base)
	{
		return static_cast<int>(std::pow(2, std::ceil(std::log(value) / log(base))));
	}
	
	//-------------------------------------------------------------------------------------

	unsigned next_power(unsigned value, unsigned base)
	{
		return static_cast<int>(std::pow(2, std::ceil(std::log(value) / log(base))));
	}
	
	//-------------------------------------------------------------------------------------

	float to_degrees(const float radians)
	{
		return (radians * 180.0f) / PI;
	}

	//-------------------------------------------------------------------------------------


	float to_radians(const float degrees)
	{
		return (degrees * PI) / 180.0f;
	}

	//-------------------------------------------------------------------------------------


	float lerp(const float start, const float end, const float t)
	{
		return start + (end - start) * t;
	}

	//-------------------------------------------------------------------------------------

	glm::vec2 lerp(const glm::vec2& start, const glm::vec2& end, const float t)
	{
		// Wrie this in terms of the float lerp to re-use code
		return glm::vec2{lerp(start.x,end.x,t), lerp(start.y, end.y,t)};
	}
	
	//-------------------------------------------------------------------------------------

	float round_to_multiple(const float value, const float multiple)
	{
		// To round the value to the nearest multiple of the given number, we just divide 
		// the value by the multiple, then round it and multiply by the  multiple. For example
		// if we want to round 8 to the nearest multiple of 5, we first divide it by 5 to get 1.6.
		// This means that one 5 fits into 8, and then 0.6 of a 5 fits into 8. Since 60% fits,
		// we are closer to 2 fitting then only 1 fitting, so by rounding 1.6 we get 2. Then multiply
		// 5 by 2 to get 10 as the next multiple. 
		
		return std::roundf(value / multiple) * multiple;
	}
	
	//-------------------------------------------------------------------------------------


	bool intersects(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& q1, const glm::vec2& q2)
	{
		// Taken from https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/

		constexpr auto orientation = [](const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
		{
			const float o = (b.y - a.y) * (c.x - a.x) - (b.x - a.x) * (c.y - b.y);

			// Points are collinear
			if(o == 0)
				return 0;
			else if(o > 0)
				return 1;
			else
				return 2;
		};

		const auto o1 = orientation(p1, p2, q1);
		const auto o2 = orientation(p1, p2, q2);
		const auto o3 = orientation(q1, q2, p1);
		const auto o4 = orientation(q1, q2, p2);

		return (o1 != o2 && o3 != o4) 
			|| (o1 == 0 && on_line_segment(p1, p2, q1))
			|| (o2 == 0 && on_line_segment(p1, p2, q2))
			|| (o3 == 0 && on_line_segment(q2, q2, p1))
			|| (o4 == 0 && on_line_segment(q1, q2, p2));
	}

	//-------------------------------------------------------------------------------------

	bool collinear(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
	{
		// If the cross product cross product of the 3 points is 0, then the points are collinear

		return std::fabsf((c.x - a.y) * (b.x - a.x) - (c.x - a.x) * (b.y - a.y)) <= std::numeric_limits<float>::epsilon();
	}
	
	//-------------------------------------------------------------------------------------

	bool on_line_segment(const glm::vec2& l1, const glm::vec2& l2, const glm::vec2& p)
	{
		const auto [min_x, max_x] = std::minmax(l1.x, l2.x);
		const auto [min_y, max_y] = std::minmax(l1.y, l2.y);

		// The point is on the line segment if its collinear with the 
		// line end points and it is within the range of the end points. 
		return collinear(l1, l2, p) && (p.x >= min_x) && (p.x <= max_x) && (p.y >= min_y) && (p.y <= max_y);
	}

	//-------------------------------------------------------------------------------------


}