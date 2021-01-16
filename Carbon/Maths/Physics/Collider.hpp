#pragma once

#include "../Models/Common.hpp"
#include "../Models/BoundingBox.hpp"
#include "../Models/BoundingCircle.hpp"
#include "../Models/BoundingTriangle.hpp"

namespace cbn
{

	//TODO: add compound collider for a collider made form colliders?

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

		
		// The API here is reversed because we don't know anything about the collider.
		// However if the API is made from a set of bounding classes then we can test
		// if we are enclosed by another collider by seeing if the collider encloses
		// all our sub-colliders. Doing it the other way around is possible but adds
		// a lot of complexity as we don't know anything about the collider. 
		virtual bool enclosed_by(const Collider& collider) const = 0;

		virtual bool encloses(const BoundingBox& box) const = 0;

		virtual bool encloses(const BoundingCircle& circle) const = 0;

		virtual bool encloses(const BoundingTriangle& triangle) const = 0;



		virtual bool contains(const Point& point) const = 0;



		virtual bool intersected_by(const Line& line) const = 0;

		virtual bool intersected_by(const Segment& segment) const = 0;

		virtual bool intersected_by(const Ray& ray) const = 0;
		
	};


}