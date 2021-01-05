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


	bool segments_intersect(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& q1, const glm::vec2& q2)
	{
		const float d = ((q2.y - q1.y) * (p2.x - p1.x)) - ((q2.x - q1.x) * (p2.y - p1.y));
		const float n1 = ((q2.x - q1.x) * (p1.y - q1.y)) - ((q2.y - q1.y) * (p1.x - q1.x));
		const float n2 = ((p2.x - p1.x) * (p1.y - q1.y)) - ((p2.y - p1.y) * (p1.x - q1.x));

		// If this condition is met, the line seg are parallel and 
		// not coincident (not the same lines). 
		if(d == 0.0f && n1 != 0.0f && n2 != 0.0f)
			return false;

		const float s = n1 / d;
		const float t = n2 / d;

		return s >= 0.0 && s <= 1.0 && t >= 0.0 && t <= 1.0;
	}

	//-------------------------------------------------------------------------------------

	bool collinear(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
	{
		// If the cross product cross product of the 3 points is 0, then the points are collinear

		return std::fabsf((c.x - a.y) * (b.x - a.x) - (c.x - a.x) * (b.y - a.y)) <= std::numeric_limits<float>::epsilon();
	}
	
	//-------------------------------------------------------------------------------------

	int signum(const float v)
	{
		return (0 < v) - (v < 0);
	}
	
	//-------------------------------------------------------------------------------------

	int line_side(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
	{
		// Return 0 if c is on the line ab, 1 is c is on its right, -1 if its on its left
		return signum((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x));
	}
	
	//-------------------------------------------------------------------------------------

	bool on_segment(const glm::vec2& l1, const glm::vec2& l2, const glm::vec2& p)
	{
		const auto [min_x, max_x] = std::minmax(l1.x, l2.x);
		const auto [min_y, max_y] = std::minmax(l1.y, l2.y);

		// The point is on the line segment if its collinear with the 
		// line end points and it is within the range of the end points. 
		return collinear(l1, l2, p) && (p.x >= min_x) && (p.x <= max_x) && (p.y >= min_y) && (p.y <= max_y);
	}

	//-------------------------------------------------------------------------------------


}