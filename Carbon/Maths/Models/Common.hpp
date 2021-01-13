#pragma once

#include <glm/glm.hpp>

namespace cbn
{
	using Point = glm::vec2;

	//TODO: expand these structs with more useful functionality

	struct Extent
	{
		Point min, max;

		bool overlaps(const Extent& other) const;

		bool contains(const Point& point) const;
	};

	struct Segment
	{
		Point p1, p2;
	};

	struct Line
	{
		Point p1, p2;
	};

	struct Ray
	{
		Point origin;
		glm::vec2 direction;
	};
}
