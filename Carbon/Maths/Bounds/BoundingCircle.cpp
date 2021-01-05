#include "BoundingCircle.hpp"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/projection.hpp>

#include "../Maths.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	void BoundingCircle::update_min_max_coords()
	{
		// The minimum and maximum coords are just the coords which are one radius away 
		// in each cardinal direction. 
		
		const glm::vec2 xy_radius = {radius(), radius()};
	
		m_MinMaxCoords = {centre() - xy_radius, centre() + xy_radius};
	}
	
	//-------------------------------------------------------------------------------------

	BoundingCircle::BoundingCircle(const float radius, const glm::vec2& local_origin)
	{
		reshape(radius, local_origin);
	}
	
	//-------------------------------------------------------------------------------------

	BoundingCircle::BoundingCircle(const Transform& transform, const float radius, const glm::vec2& local_origin)
		: m_Transform(transform)
	{
		// Remove any scaling from the transform, and apply it directly to the radius.
		// Note that the reshape function will transform the circle to m_Transform so we 
		// don't need to do it manually here. 
		m_Transform.scale_to(1);
		reshape(radius * transform.scale().x, local_origin);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::overlaps(const BoundingBox& box) const
	{
		// Complete the calculation from the boxes side its implementation is faster
		// than what can be implemented from the circle's side. 
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

	bool BoundingCircle::encloses(const BoundingBox& box) const
	{
		// A circle encloes a box, if all its vertices are within the circle
		const auto& mesh = box.mesh();
		return contains_point(mesh.vertex_1)
			&& contains_point(mesh.vertex_2)
			&& contains_point(mesh.vertex_3)
			&& contains_point(mesh.vertex_4);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::encloses(const BoundingCircle& circle) const
	{
		// If this circle encloses the given circle, then the distance to
		// the centre of the circle plus the circles radius should be less 
		// than the radius of this circle. That is, the circles furthest point
		// should be within this circle. 
		const float max_distance = radius();
		const float radius = circle.radius();
		return glm::distance2(centre(), circle.centre()) <= (max_distance - radius) * (max_distance - radius);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::encloses(const BoundingTriangle& triangle) const
	{
		// The circle encloes the triangle if all its vertices are within
		// the circle
		const auto& mesh = triangle.mesh();
		return contains_point(mesh.vertex_1)
			&& contains_point(mesh.vertex_2)
			&& contains_point(mesh.vertex_3);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::contains_point(const glm::vec2& point) const
	{
		// The point is within the circle if its distance from the circle
		// centre is less than the circle's radius.
		
		const float radius = this->radius();
		return glm::distance2(centre(), point) <= radius * radius;
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::is_intersected_by_line(const glm::vec2& p1, const glm::vec2& p2) const
	{
		// If we set the equation of the line equal to the circle's equation, we form a quadratic
		// equation whose solutions are the intersection coordinates. We only care about whether the
		// line intersects or not, so we can skip the calculation of the quadratic and simply 
		// check whether it has any solutions by checking whether the determinant is >= 0
		// Maths from: https://mathworld.wolfram.com/Circle-LineIntersection.html
		
		const float radius = this->radius();
		return radius * radius * glm::distance2(p1, p2) - (p1.x * p2.y - p2.x * p1.y) >= 0;
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::is_intersected_by_line_segment(const glm::vec2& p1, const glm::vec2& p2) const
	{
		// Project the vector from p1 to the circle centre onto the line segment. This will give
		// the closest point to the circle on the infinite line formed by p1 and p2. If this point
		// lies on the segment, and the distance between the point and circle centre is less than 
		// the radius of the circle, then the circle and line segment intersect. 

		const float radius = this->radius();
		const glm::vec2 closest_point = glm::proj(centre() - p1, p2 - p1);

		return on_segment(p1, p2, closest_point) && glm::distance2(closest_point, centre()) <= radius * radius;
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::is_intersected_by_ray(const glm::vec2& origin, const glm::vec2& towards) const
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

		return is_intersected_by_line(origin, towards) && (contains_point(origin) || glm::dot(centre() - origin, towards - origin) >= 0);
	}
	
	//-------------------------------------------------------------------------------------

	void BoundingCircle::reshape(const float radius, const glm::vec2& local_origin)
	{
		m_Radius = radius;
	
		// The circle's origin is at (0,0) in local space. So we need to offset the
		// centre of the circle such that the given local origin offset from the 
		// centre has the origin at (0,0). Since by default the local circle centre is at (0,0),
		// we can just do this by offseting the centre by the opposite offset of the given origin offset.
		m_LocalCentre = -local_origin;

		// With the local centre changed, we need to re-transform the circle
		transform_to(m_Transform);
	}
	
	//-------------------------------------------------------------------------------------

	void BoundingCircle::transform_to(const Transform& transform)
	{
		// Update the transform and remove any scaling as the
		// scaling is absorbed by the radius.
		m_Transform = transform;
		m_Transform.scale_to(1);
		m_Radius *= transform.scale().x;

		// We only need to transform the centre for 
		// the circle as it has no vertices. 
		m_Centre = transform.apply(m_LocalCentre);

		// Update the min max coords of the circle, 
		// since the circle has now changed position or size.
		update_min_max_coords();
	}
	
	//-------------------------------------------------------------------------------------

	void BoundingCircle::transform_by(const Transform& transform)
	{
		transform_to(transform.apply(m_Transform));
	}
	
	//-------------------------------------------------------------------------------------

	const glm::vec2& BoundingCircle::centre() const
	{
		return m_Centre;
	}
	
	//-------------------------------------------------------------------------------------

	const glm::vec2& BoundingCircle::origin() const
	{
		// The origin states where the translation of the circle
		// is taken from, so its origin is just its translation. 
		return m_Transform.translation();
	}
	
	//-------------------------------------------------------------------------------------

	const float BoundingCircle::radius() const
	{
		return m_Radius;
	}
	
	//-------------------------------------------------------------------------------------

	BoundingBox BoundingCircle::wrap_axis_alligned() const
	{
		// Use the minimum and maximum coords of the circle 
		// to define the AABBs size. The box would simply be
		// translated so that its centre matches the centre
		// of the circle. The circle's origin is not preserved. 
		
		const auto& [min, max] = min_max_coords();
		return {Transform{centre()}, max - min};
	}
	
	//-------------------------------------------------------------------------------------

	const std::tuple<glm::vec2, glm::vec2>& BoundingCircle::min_max_coords() const
	{
		return m_MinMaxCoords;
	}
	
	//-------------------------------------------------------------------------------------

}
