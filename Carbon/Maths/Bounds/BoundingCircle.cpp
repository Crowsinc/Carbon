#include "BoundingCircle.hpp"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/projection.hpp>

#include "../Maths.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	BoundingCircle::BoundingCircle(const float radius)
		: m_Radius(radius) {}
	
	//-------------------------------------------------------------------------------------

	BoundingCircle::BoundingCircle(const Transform& transform, const float radius)
		: m_Radius(0),
		Transformable(transform) {}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::overlaps(const BoundingArea& area) const
	{
		return area.overlaps(*this);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingCircle::overlaps(const BoundingBox& box) const
	{
		// If the box is axis alligned, then the overlap check is easy. Just check if the closest point
		// to the circle from the perimeter of the box is less than 1 radius away from the circle. 
		// This idea was taken from: https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection

		glm::vec2 circle_centre = centre(), box_centre = box.centre();

		// Before that however, we can skip all the calculations if the circle's centre is within the box
		if(box.contains_point(centre()))
			return true;

		// If the box is not axis alligned. Since the circle collision is independent of its
		// orientation, we can rotate the coordinate system about the origin so that the vertical axis
		// now lines up with one of the box's edges, thus making it axis alligned in the new coordinate
		// system. We can then perform the collision which the centres in the new coordinate system,
		// as if the box were always axis alligned. 
		if(!box.is_axis_alligned())
		{
			// We first get a unit vector in the direction of one of the box's edges and find its
			// angle with the normal vertical axis. Note that we only need to look at one of the edges because
			// the two edges we care about are perpendicular just like the axes, so as long as one of the axes
			// lines up the other will too.

			const auto& box_vertices = box.vertices();
			const glm::vec2 unit_edge = glm::normalize(box_vertices.vertex_1 - box_vertices.vertex_2);

			// Use the dot product to find the angle between the unit edge vector and the unit j vector.
			// Note that we don't need to include the lengths of the vectors because they are unit vectors. 
			const float angle = std::acosf(glm::dot(unit_edge, {0, 1}));

			// We now want to define a new coordinate system which is simply the normal cartesian coordinate system
			// but rotated by this angle so that the axes are alligned with the box edges. Then convert the coordinates
			// of the box and circle to use this new coordinate system. 

			//CHECKKK MMMMMMMMEEEEEEEEEEEEE: its possible we are applying the angle in the wrong direction (i.e. counter clockwise instead of clockwise) _____________________________________
			Transform system(0, 0, angle);
			circle_centre = system.apply(circle_centre);
			box_centre = system.apply(box_centre);
		}

		// Now perform the normal aabb collision test. 
		const float radius = scaled_radius();
		const glm::vec2 box_half_size = box.scaled_size() * 0.5f;
		const glm::vec2 closest_box_point = box_centre + glm::clamp(box_centre - circle_centre, -box_half_size, box_half_size);

		return glm::distance2(circle_centre, closest_box_point) <= radius * radius;
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::overlaps(const BoundingCircle& circle) const
	{
		// This is quite simple, just check if the distance between the two centres is less than the sum of both radii
		const float radius_1 = scaled_radius();
		const float radius_2 = circle.scaled_radius();
		return glm::distance2(centre(), circle.centre()) <= (radius_1 + radius_2) * (radius_1 + radius_2);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::encloses(const BoundingBox& box) const
	{
		// If the box is contained within the circle, simply check 
		// that all vertices of the box are within the circle
		const auto& verts = box.vertices();

		return contains_point(verts.vertex_1) 
			&& contains_point(verts.vertex_2)
			&& contains_point(verts.vertex_3)
			&& contains_point(verts.vertex_4);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::encloses(const BoundingCircle& circle) const
	{
		// If the circle is enclosed by this one, then the distance between the centre of this circle
		// and the centre of the other plus its radius should be less than the radius of this circle.
		// That is, the furthest point of the circle is no further than the circumference of this circle. 
		const float enclosed_radius = circle.scaled_radius();
		const float enclosing_radius = circle.scaled_radius();
		return glm::distance2(centre(), circle.centre()) <= (enclosing_radius - enclosed_radius) * (enclosing_radius - enclosed_radius);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::contains_point(const glm::vec2& point) const
	{
		// Simply check that the distance to the point is less than the radius
		const auto radius = scaled_radius();
		return glm::distance2(centre(), point) <= radius * radius;
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::is_intersected_by_line(const glm::vec2& p1, const glm::vec2& p2) const
	{
		// Simply obtain the projection of the vector from p1 to the circle centre
		// onto the vector p1p2. This will give a point on the same infinite line as p1p2 which 
		// has the smallest distance from the circle to the line segment. If the distance of this
		// line is less than theradius, ther is an intersection. Note however that the projection
		// is not guaranteed to be within the segment, only on the same line so we need to check
		// that the closest point is on the segment too. Doing it this way avoids square roots making it fast. 

		const float radius = scaled_radius();

		const glm::vec2 segment = p2 - p1;
		const glm::vec2 circle_centre = centre();
		const glm::vec2 segment_to_circle = circle_centre - p1;
		const glm::vec2 closest_point = glm::proj(segment_to_circle, segment);

		// Note that we need to ensure that the point is on the segment, because the projection can
		// be backwards if the circle is in the opposite direction as the segment form p1 to p2. 
		return on_line_segment(p1, p2, closest_point) && glm::distance2(closest_point, circle_centre) <= radius * radius;
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingCircle::is_intersected_by_ray(const glm::vec2& origin, const glm::vec2& towards) const
	{
		// Treat the ray as an infinite line in the direction between the origin and the towards
		// vector. Project the vector from the origin to the circle centre onto this line to
		// find the closest point on this line to the circle. If the ray vector and the vector
		// from the origin to the closest point are in the same direction, then the point
		// must be on the ray so just do a collision test using a distance test. 

		const float radius = scaled_radius();
	
		const glm::vec2 circle_centre = centre();
		const glm::vec2 ray_direction = towards - origin;
		const glm::vec2 origin_to_circle = circle_centre - origin;
		const glm::vec2 closest_point = glm::proj(origin_to_circle, ray_direction);

		return glm::dot(closest_point, ray_direction) > 0 && glm::distance2(closest_point, circle_centre) <= radius * radius;
	}
	
	//-------------------------------------------------------------------------------------

	BoundingCircle BoundingCircle::wrap_as_bounding_circle() const
	{
		return *this;
	}
	
	//-------------------------------------------------------------------------------------

	BoundingBox BoundingCircle::wrap_as_bounding_box() const
	{
		// Wrap the circle in an axis alligned bounding box, then 
		// rotate it to have the same rotation as the circle
		auto bounding_box = wrap_as_axis_alligned();
		bounding_box.rotate_to(rotation_degrees());
		return bounding_box;
	}
	
	//-------------------------------------------------------------------------------------

	BoundingBox BoundingCircle::wrap_as_axis_alligned() const
	{
		// Clone the transform for the circle for the bounding box, but
		// remove its rotation to make it axis alligned. 
		auto transform = as_transform();
		transform.rotate_to(0);

		// Simply make a bounding box whose size is equal to the diamter of the circle
		// We don't use scaled diameter as the scaling is preserved in the transform. 
		return BoundingBox{transform, {diameter(), diameter()}};
	}
	
	//-------------------------------------------------------------------------------------

	glm::vec2 BoundingCircle::centre() const
	{
		return translation();
	}
	
	//-------------------------------------------------------------------------------------

	float BoundingCircle::radius() const
	{
		return m_Radius;
	}
	
	//-------------------------------------------------------------------------------------

	float BoundingCircle::diameter() const
	{
		return radius() * 2.0f;
	}
	
	//-------------------------------------------------------------------------------------

	float BoundingCircle::scaled_radius() const
	{
		return radius() * scale();
	}
	
	//-------------------------------------------------------------------------------------

	float BoundingCircle::scaled_diameter() const
	{
		return scaled_radius() * 2.0f;
	}
	
	//-------------------------------------------------------------------------------------

}