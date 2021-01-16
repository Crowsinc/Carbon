#include "BoundingCircle.hpp"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/projection.hpp>

#include "../Maths.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	void BoundingCircle::update_extent() const
	{
		// The minimum and maximum coords are just the coords which are one radius
		// from the centre in each cardinal direction.

		const glm::vec2 xy_radius = {radius(), radius()};
		const auto& centre = this->centre();

		m_Extent.min = centre - xy_radius;
		m_Extent.max = centre + xy_radius;
	}
	
	//-------------------------------------------------------------------------------------

	BoundingCircle::BoundingCircle(const float radius)
		: m_LocalCentreOffset(0,0)
	{
		resize(radius);
	}
	
	//-------------------------------------------------------------------------------------

	BoundingCircle::BoundingCircle(const Transform& transform, const float radius)
		: Transformable(transform),
		  m_LocalCentreOffset(0, 0)
	{
		resize(radius);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::overlaps(const Collider& collider) const
	{
		// Use the collider's overlap method for circles.
		return collider.overlaps(*this);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::overlaps(const BoundingBox& box) const
	{
		// Complete the calculation from the boxes side as its implementation 
		// is faster than what can be implemented from the circle's side. 
		return box.overlaps(*this);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::overlaps(const BoundingCircle& circle) const
	{
		// Two circles overlap if the distance between their centres 
		// is less than the sum of their radii. 
		const float radius_1 = radius(), radius_2 = circle.radius();
		return glm::distance2(centre(), circle.centre()) <= (radius_1 + radius_2) * (radius_1 + radius_2);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::overlaps(const BoundingTriangle& triangle) const
	{
		// Do this from the triangle side as its implementation is faster. 
		return triangle.overlaps(*this);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::enclosed_by(const Collider& collider) const
	{
		return collider.encloses(*this);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::encloses(const BoundingBox& box) const
	{
		// A circle encloses a box, if all its vertices are within the circle
		const auto& mesh = box.mesh();
		return contains(mesh.vertices[0])
			&& contains(mesh.vertices[1])
			&& contains(mesh.vertices[2])
			&& contains(mesh.vertices[3]);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::encloses(const BoundingCircle& circle) const
	{
		// For the given circle B to be enclosed within this one, then
		// the furthest point of the circle B from the centre of this circle,
		// must be within this circle. So we take the distance between the 
		// centre of this circle and circle B, and add circle B's radius
		// to get its furthest point's distance. If this distance falls 
		// below the radius of this circle, then it is enclosed. 
		
		// Note however that we re-arrange the formula so that we can use the distance 
		// squared to avoid square roots. If the distance between the centres is D,
		// the circle Bs radius is r and our radius is R, the formula goes from:
		//						D + r <= R
		// to
		//						D^2 <= (R - r)(R - r)
		// However, if R - r is negative then the ineqaulity needs to be
		// reversed. This is avoided by just returning false if the radius of circle
		// B is greater than ours, as it would be imposisble to enclose cirlce B.

		const float distance_squared = glm::distance2(centre(), circle.centre());
		const float radial_difference = radius() - circle.radius();

		return radial_difference > 0 && distance_squared <= radial_difference * radial_difference;
	}
	


	//-------------------------------------------------------------------------------------

	bool BoundingCircle::encloses(const BoundingTriangle& triangle) const
	{
		// The circle encloes the triangle if all its vertices are within
		const auto& mesh = triangle.mesh();
		return contains(mesh.vertices[0])
			&& contains(mesh.vertices[1])
			&& contains(mesh.vertices[2]);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::contains(const Point& point) const
	{
		// The point is within the circle if its distance from the circle
		// centre is less than the circle's radius.
		
		const float radius = this->radius();
		return glm::distance2(centre(), point) <= radius * radius;
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::intersected_by(const Line& line) const
	{
		// Project a vector from p1 to the circle centre along the infinite line formed by p1 and
		// p2. The resulting projection will lead to the closest point on the line to the circle 
		// centre. If this point is inside the circle, then the circle is intersected by the line.
		// We could also do this by making the equation of the circle equal to the equation of the
		// line, giving a quadratic whose roots are the intersection points. We would then simply
		// check the descriminant of the quadratic to check if it has a solution or not. 

		const auto& centre = this->centre();
		const float radius = this->radius();
		const auto closest_point = line.p1 + glm::proj(centre - line.p1, line.p2 - line.p1);
		
		return contains(closest_point);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::intersected_by(const Segment& segment) const
	{
		// Find the point the line segment which is closes to the circle's centre. If the distance 
		// between the point and circle centre is less than the radius of the circle, then the 
		// circle and line segment intersect.

		const float radius = this->radius();
		const auto closest_point = closest_segment_point(segment.p1, segment.p2, centre());

		return contains(closest_point);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::intersected_by(const Ray& ray) const
	{
		// A ray is simply an infinite line that was cut in half at some point. To check if the ray 
		// intersects the circle, first treat the ray as an infinite line and see if that intersects
		// the circle. If it does, then we need to ensure that the part of the line which intersects
		// is not the half which doesnt exist on the ray. To do this, ensure that the ray and the 
		// vector from the ray origin to the circle are in the same direction. This is done by 
		// checking if their dot product is greater than 0, hence the vectors for an acute angle.
		// It is possible that the the circle centre may be in the opposite direction of the ray, 
		// but still intersect the ray. This only occurs when the origin is inside the circle. 
		// Note that his only works because the circle is perfectly symmetrical. If it where 
		// a box, its possible for the centre to be in the direction of the ray, but the 
		// intersection point is on the infinite line which is not part of the ray. 

		return intersected_by(Line{ray.origin, ray.origin + ray.direction})
			&& (contains(ray.origin) || glm::dot(centre() - ray.origin, ray.direction) >= 0);
	}
	
	//-------------------------------------------------------------------------------------

	const Extent& BoundingCircle::extent() const
	{
		update_extent();

		return m_Extent;
	}
	
	//-------------------------------------------------------------------------------------

	void BoundingCircle::resize(const float radius)
	{
		m_Radius = radius;
	
		// We can update the extent here without caching because its very fast
		update_extent();
	}
	
	//-------------------------------------------------------------------------------------

	void BoundingCircle::specify_origin(const glm::vec2& origin_offset, const bool local_coords)
	{
		const Point old_centre = centre();

		// The origin specifies the origin of the box as an offset from the centre of the circle.
		// If the local_coords flag is set, then the offset is specified in the local space of 
		// the circle. Otherwise its specified using world coordinates. 
		if(!local_coords)
		{
			// If the offset is given in world coordinates, then we need to 
			// project the offset along the local axes of the circle to turn 
			// them into local coordiantes. Since the direction vector and its 
			// normal line up with the axes and are unit normals, we can do 
			// this with just two dot products. 
			const auto& dir = direction();
			const glm::vec2 dir_normal{dir.y, -dir.x};

			m_LocalCentreOffset.x = -glm::dot(origin_offset, dir_normal);
			m_LocalCentreOffset.y = -glm::dot(origin_offset, dir);
		}
		else m_LocalCentreOffset = -origin_offset;

		// The translation is taken from the origin, so if the origin is changed then the circle 
		// would move so that the new origin is placed (in world coordinates) where the old 
		// origin was. For this reason, we need to offset the translation so that the circle stays 
		// with the same location in the world. The translation amount has to be obtained from the difference 
		// in position after transformation (non-local). The easiest way to do this is to observe the
		// difference in the transformed centre after the origin change, and translate the circle by 
		// the opposite change to cancel it out. 
		translate_by(old_centre - centre());
	}
	
	//-------------------------------------------------------------------------------------

	const Point& BoundingCircle::centre() const
	{
		//TODO: caching

		m_Centre = as_transform().apply(m_LocalCentreOffset);

		return m_Centre;
	}
	
	//-------------------------------------------------------------------------------------

	const Point& BoundingCircle::direction() const
	{
		//TODO: caching
		
		// The local direction is from the centre upwards one unit
		// So specify the local direction as a point that is one unit 
		// above the local centre, then transform it and find the new direction
		// between this point and the transformed centre.
		constexpr glm::vec2 unit_j{0.0f, 1.0f};
		m_Direction = as_transform().apply(m_LocalCentreOffset + unit_j) - centre();

		return m_Direction;
	}
	
	//-------------------------------------------------------------------------------------

	const glm::vec2& BoundingCircle::origin() const
	{
		// The origin states where the translation of the circle
		// is taken from, so its origin is just its translation. 
		return translation();
	}
	
	//-------------------------------------------------------------------------------------

	const float BoundingCircle::radius() const
	{
		return m_Radius;
	}
	
	//-------------------------------------------------------------------------------------

	BoundingBox BoundingCircle::wrap_axis_alligned() const
	{
		// Simply create a bounding box from the extent of the circle
		return {extent()};
	}
	
	//-------------------------------------------------------------------------------------

}
