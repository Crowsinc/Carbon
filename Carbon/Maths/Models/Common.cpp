#include "Common.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	bool Extent::overlaps(const Extent& other) const
	{
		// The extents overlap if the min/max coords overlap on both axes. 
		return (max.x >= other.min.x) && (min.x <= other.max.x)
			&& (max.y >= other.min.y) && (min.y <= other.max.y);
	}

	//-------------------------------------------------------------------------------------
	
	bool Extent::contains(const Point& point) const
	{
		return point.x >= min.x
			&& point.x <= max.x
			&& point.y <= max.y
			&& point.y >= min.y;
	}

	//-------------------------------------------------------------------------------------

}