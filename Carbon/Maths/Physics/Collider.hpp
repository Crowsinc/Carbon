#pragma once

#include "../Models/Common.hpp"
#include "../Models/BoundingBox.hpp"
#include "../Models/BoundingCircle.hpp"
#include "../Models/BoundingTriangle.hpp"

namespace cbn
{

	class BoundingBox;
	class BoundingCircle;
	class BoundingTriangle;

	struct Collider
	{
		virtual const Extent& extent() const = 0;

		// MUST NOT PUSH THE OVERLAP TEST TO ANOTHER CLASS OR WILL CAUSE INF RECURSION
		virtual bool overlaps(const Collider& collider) const = 0;

		virtual bool overlaps(const BoundingBox& box) const = 0;

		virtual bool overlaps(const BoundingCircle& circle) const = 0;

		virtual bool overlaps(const BoundingTriangle& triangle) const = 0;

		
		virtual bool encloses(const Collider& collider) const = 0;

		virtual bool encloses(const BoundingBox& box) const = 0;

		virtual bool encloses(const BoundingCircle& circle) const = 0;

		virtual bool encloses(const BoundingTriangle& triangle) const = 0;



		virtual bool contains(const Point& point) const = 0;



		virtual bool intersected_by(const Line& line) const = 0;

		virtual bool intersected_by(const Segment& segment) const = 0;

		virtual bool intersected_by(const Ray& ray) const = 0;
		
	};


}