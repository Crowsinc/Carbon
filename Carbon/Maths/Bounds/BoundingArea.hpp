#pragma once

#include <glm/glm.hpp>

namespace cbn
{

	//TODO: bounding triangle, AxisAllignedBoundingBox?
	//TODO: enforce origin changing functionality

	class BoundingCircle;
	class BoundingBox;
	class BoundingTriangle;

	class BoundingArea 
	{
	public:

		virtual bool overlaps(const BoundingArea& area) const = 0;

		virtual bool overlaps(const BoundingBox& box) const = 0;

		virtual bool overlaps(const BoundingCircle& circle) const = 0;

//		virtual bool overlaps(const BoundingTriangle& triangle) const = 0;

		virtual bool encloses(const BoundingBox& box) const = 0;

		virtual bool encloses(const BoundingCircle& circle) const = 0;

//		virtual bool encloses(const BoundingTriangle& triangle) const = 0;

		virtual bool contains_point(const glm::vec2& point) const = 0;

		virtual bool is_intersected_by_line(const glm::vec2& p1, const glm::vec2& p2) const = 0;

		virtual bool is_intersected_by_ray(const glm::vec2& origin, const glm::vec2& towards) const = 0;

		//virtual BoundingTriangle wrap_as_bounding_triangle() const = 0;

		virtual BoundingCircle wrap_as_bounding_circle() const = 0;

		virtual BoundingBox wrap_as_bounding_box() const = 0;

		virtual BoundingBox wrap_as_axis_alligned() const = 0;

	};
}