#include "BoundingBox.hpp"

#include <algorithm>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "../Maths.hpp"
#include "../Matrix.hpp"
#include "../../Diagnostics/Assert.hpp"

namespace cbn
{
	
	//-------------------------------------------------------------------------------------

	Point BoundingBox::transform_to_local(const Point& point) const
	{
		// Transform a point to be in the local coordinate system of the box, where the box is AABB and 
		// is centred on (0,0). This works because the point has no orientation, hence we can effectly 
		// transform the coordinate system to allign it with the rotated box and treat it as AABB. However, 
		// in this case we do the opposite and transform the point as if it were the boxes' local coordinate system. 
		
		// To do this, find the vector between the box centre and point and project it
		// across two perpendicular normals of the box. This will give us the offset from 
		// the boxes' centre to the point along these normals. The normals are alligned
		// with the boxes edges so, if we get the offset along the normals we get the
		// offset along the coordinate system whose axes are alligned with the boxes.
		// We can then use this to transform the point to be in the boxes local 
		// coordinate system where the box is AABB. 

		// NOTE: we use the centre, not the origin, as it can speed up some of the calculations
		// used by the bounding box. 

		const auto& mesh = this->mesh();
		const glm::vec2 centre_to_point = point - centre();

		// Get the scalar projection of the origin to point vector across two
		// perpendicular normals. Since the normals are unit vectors, we can
		// simplify the projection to a dot product, avoiding any square roots.
		return {glm::dot(centre_to_point, mesh.normal_4), glm::dot(centre_to_point, mesh.normal_1)};
	}
	
	//-------------------------------------------------------------------------------------

	void BoundingBox::update_extent() const
	{
		// To find the minimum and maximum x and y coords, we can just make use of the 
		// rotation of the bounding box along with the properties of a rectangle. Depending on which 
		// quadrant the box is facing towards, we can instantly determine which vertex of the box 
		// will be the topmost, leftmost, rightmost and bottommost. This is possible because the
		// sides of the rectangle are either perpendicular or parallel to each other. 

		// Note that the rotation goes counter clockwise, 
		// so a 90 degree rotation is 90 degrees counter clockwise
		const auto rotation = rotation_degrees();

		float min_x = 0, max_x = 0, min_y = 0, max_y = 0;

		const auto& mesh = this->mesh();

		if(rotation >= 0 && rotation < 90)
		{
			// vertex 1 is top, vertex 2 is left, vertex 3 is bottom, vertex 4 is right
			max_x = mesh.vertex_4.x;
			min_x = mesh.vertex_2.x;
			max_y = mesh.vertex_1.y;
			min_y = mesh.vertex_3.y;
		}
		else if(rotation >= 90 && rotation < 180)
		{
			// vertex 1 is left, vertex 2 is bottom, vertex 3 is right, vertex 4 is top
			min_x = mesh.vertex_1.x;
			max_x = mesh.vertex_3.x;
			min_y = mesh.vertex_2.y;
			max_y = mesh.vertex_4.y;
		}
		else if(rotation >= 180 && rotation < 270)
		{
			// vertex 1 is bottom, vertex 2 is right, vertex 3 is top, vertex 4 is left
			max_x = mesh.vertex_2.x;
			min_x = mesh.vertex_4.x;
			max_y = mesh.vertex_3.y;
			min_y = mesh.vertex_1.y;
		}
		else if(rotation >= 270 && rotation < 360)
		{
			// vertex 1 is right, vertex 2 is top, vertex 3 is left, vertex 4 is bottom
			min_x = mesh.vertex_3.x;
			max_x = mesh.vertex_1.x;
			min_y = mesh.vertex_4.y;
			max_y = mesh.vertex_2.y;
		}
		else CBN_Assert(false, "The rotation is not wrapping between 0 and 360 as expected");

		m_Extent.min = {min_x, min_y};
		m_Extent.max = {max_x, max_y};
	}
	
	//-------------------------------------------------------------------------------------

	BoundingBox::BoundingBox(const glm::vec2& size)
		: m_LocalOriginOffset(0,0)
	{
		resize(size);
	}
	
	//-------------------------------------------------------------------------------------

	BoundingBox::BoundingBox(const Extent& extent)
		: BoundingBox(extent.max - extent.min)
	{
		translate_to((extent.min + extent.max) * 0.5f);
	}
	
	//-------------------------------------------------------------------------------------

	BoundingBox::BoundingBox(const Transform& transform, const glm::vec2& size)
		: Transformable(transform),
		  m_LocalOriginOffset(0, 0)
	{
		resize(size);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::overlaps(const Collider& collider) const
	{
		// Use the box overlap function of the collider. 
		return collider.overlaps(*this);;
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::overlaps(const BoundingBox& box) const
	{
		// If both boxes are axis alligned, do a simple extent test.
		if(is_axis_alligned() && box.is_axis_alligned())
			return extent().overlaps(box.extent());
		else
		{
			// If we are here, we have to use the separating axis theorem on both boxes
			// Note that since boxes have two parallel sides, we only have to check 2 axes. 
			// Which should be a lot faster than the normal SAT which would check 4. 

			const auto& mesh_1 = mesh();
			const auto& mesh_2 = box.mesh();

			// Get the axes of the boxes by just using two of their differing normals. 
			const std::array<glm::vec2, 4> axes = {
				mesh_1.normal_1, mesh_1.normal_4, 
				mesh_2.normal_1, mesh_2.normal_4,
			};

			return sat_test(axes, mesh_1.vertices, mesh_2.vertices);
		}
	}

	//-------------------------------------------------------------------------------------

	bool BoundingBox::overlaps(const BoundingCircle& circle) const
	{
		// The circle has no orientation, so we can transform its centre to be
		// in the local space where the box is AABB and is centred on (0,0). 
		// We can then treat this as a circle - AABB collision even if the box 
		// is not AABB. 

		const glm::vec2 local_circle_centre = transform_to_local(circle.centre());

		// The circle to AABB collision test is done by finding the closest point in
		// the box to the circle and seeing if the distance between that point and
		// the circle's centre is less than its radius. We find the closest point by
		// getting a vector from the box centre to the circle centre and clamping
		// its length by the half extent of the box. This effectively clamps the 
		// vector so that it ends within the rectangle, hence is the closest point
		// that is still inside the rectangle. 

		const float radius = circle.radius();
		const glm::vec2 box_half_size = size() * 0.5f;
		const glm::vec2 closest_box_point = glm::clamp(local_circle_centre, -box_half_size, box_half_size);

		return glm::distance2(local_circle_centre, closest_box_point) <= radius * radius;
	}

	//-------------------------------------------------------------------------------------

	bool BoundingBox::overlaps(const BoundingTriangle& triangle) const
	{
		// This uses more triangle maths, so do it from the triangle's side
		return triangle.overlaps(*this);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::enclosed_by(const Collider& collider) const
	{
		return collider.encloses(*this);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::encloses(const BoundingBox& box) const
	{
		// The box is enclosed if all its points are within this box
	
		const auto& mesh = box.mesh();
		return contains(mesh.vertex_1) 
			&& contains(mesh.vertex_2) 
			&& contains(mesh.vertex_3)
			&& contains(mesh.vertex_4);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::encloses(const BoundingCircle& circle) const
	{
		// The box encloses the circle if the circle's centre is inside the box, 
		// and the circle is not intersected by any of the box's edges. 

		// Note: This can probably be optimised mathematically if it becomes a problem.
		const auto& mesh = this->mesh();
		return contains(circle.centre())
			&& !circle.intersected_by(mesh.edge_1)
			&& !circle.intersected_by(mesh.edge_2)
			&& !circle.intersected_by(mesh.edge_3)
			&& !circle.intersected_by(mesh.edge_4);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingBox::encloses(const BoundingTriangle& triangle) const
	{
		// The box encloses the triangle if all its vertices are inside the box

		const auto& mesh = triangle.mesh();
		return contains(mesh.vertex_1) 
			&& contains(mesh.vertex_2)
			&& contains(mesh.vertex_3);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::contains(const Point& point) const
	{
		// Simply transform the point to local space where the box is AABB with its 
		// centre at (0,0) and do a fast bounds check.

		const auto local_point = transform_to_local(point);

		return m_LocalExtent.contains(local_point);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::intersected_by(const Line& line) const
	{
		// The box is intersected by the line if not all vertices lie on the same side of the line
		// Or one of the vertices is directly intersected by the line. 

		const auto& mesh = this->mesh();

		// The line side function returns +1 if the vertex is right of the line, -1 if its on 
		// its left and 0 if its on the line. So for the box to be intersected, at least one
		// vertex has to be left of the line, and at least one must be on its right. Or one
		const auto s1 = line_side(line.p1, line.p2, mesh.vertex_1);
		const auto s2 = line_side(line.p1, line.p2, mesh.vertex_2);
		const auto s3 = line_side(line.p1, line.p2, mesh.vertex_3);
		const auto s4 = line_side(line.p1, line.p2, mesh.vertex_4);

		return (s1 >= 0 || s2 >= 0 || s3 >= 0 || s4 >= 0) && (s1 <= 0 || s2 <= 0 || s3 <= 0 || s4 <= 0);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::intersected_by(const Segment& segment) const
	{
		// First check if the infinite line formed by the segment intersects the box. If the infinite 
		// line doesn't intersect, then the segment definitely doesn't. If it does intersect, we need
		// to ensure that the intersection point lies on the segment. We do this by checking if the
		// line overlaps both axes of the box. Since it has to overlap the axes of the both, we 
		// transform the line to be in the boxes local space where the box is AABB in order to 
		// make the calculation easier. 
		
		const auto local_p1 = transform_to_local(segment.p1);
		const auto local_p2 = transform_to_local(segment.p2);
		
		const auto [min_x, max_x] = std::minmax(local_p1.x, local_p2.x);
		const auto [min_y, max_y] = std::minmax(local_p1.y, local_p2.y);
		
		const glm::vec2 min{min_x, min_y};
		const glm::vec2 max{max_x, max_y};

		return intersected_by(Line{segment.p1, segment.p2})
			&& m_LocalExtent.overlaps(Extent{min, max});
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::intersected_by(const Ray& ray) const
	{
		// To test this we first transform the ray to be in local coordinates so that the box is AABB.
        // Then we perform the simple branchless slab test. 
		
		const glm::vec2 local_ray_origin = transform_to_local(ray.origin);
		const glm::vec2 local_ray_dir = transform_to_local(ray.origin + ray.direction) - local_ray_origin;
		const glm::vec2 inverse_local_dir = {1.0f / local_ray_dir.x, 1.0f / local_ray_dir.y};

		const auto t0 = (m_LocalExtent.min - local_ray_origin) * inverse_local_dir;
		const auto t1 = (m_LocalExtent.max - local_ray_origin) * inverse_local_dir;

		// TODO: use minmax
		const auto t_min = glm::compMax(glm::min(t0, t1));
		const auto t_max = glm::compMin(glm::max(t0, t1));

		return t_max >= 0 && t_min <= t_max;
	}
	
	//-------------------------------------------------------------------------------------

	const Extent& BoundingBox::extent() const
	{
		//TODO: caching

		update_extent();

		return m_Extent;
	}
	
	//-------------------------------------------------------------------------------------

	void BoundingBox::resize(const glm::vec2& size)
	{
		m_Size = size;

		// The local extent is the extent of the box with no rotation and centered at (0,0)
		const auto half_size = size * 0.5f;
		m_LocalExtent.min = -half_size;
		m_LocalExtent.max = half_size;

		//TODO: handle with caching instead
		// The mesh will need updating, so will the extents etc.
		m_Mesh = QuadMesh::Create(as_transform(), m_Size, m_LocalOriginOffset);
		update_extent();
	}

	//-------------------------------------------------------------------------------------

	void BoundingBox::specify_origin(const glm::vec2& origin_offset, bool local_coords)
	{
		const Point old_centre = centre();

		// The origin specifies the origin of the box as an offset from the centre of the box.
		// If the local_coords flag is set, then the offset is specified in the local space of 
		// the box. Otherwise its specified using world coordinates. 
		if(!local_coords)
		{
			// If the offset is given in world coordinates, then we need to 
			// project the offset along the local axes of the box to turn 
			// them into local coordiantes. Since the normals 4 and 1 line up 
			// with the axes and are unit normals, we can do this with just
			// two dot products. 
			const auto& mesh = this->mesh();
			m_LocalOriginOffset.x = glm::dot(origin_offset, mesh.normal_4);
			m_LocalOriginOffset.y = glm::dot(origin_offset, mesh.normal_1);
		} else m_LocalOriginOffset = origin_offset;

		// The translation is taken from the origin, so if the origin is changed then the box 
		// would move so that the new origin is placed (in world coordinates) where the old 
		// origin was. For this reason, we need to offset the translation so that the box stays 
		// with the same location in the world. The translation amount has to be obtained from the difference 
		// in position after transformation (non-local). The easiest way to do this is to observe the
		// difference in the transformed centre after the origin change, and translate the box by 
		// the opposite change to cancel it out. 
		translate_by(old_centre - centre());
	}
	
	//-------------------------------------------------------------------------------------

	const glm::vec2& BoundingBox::direction() const
	{
		// The direction is alligned with normal 1, which faces north in local space
		const auto& mesh = this->mesh();
		return mesh.normal_1;
	}

	//-------------------------------------------------------------------------------------

	const Point& BoundingBox::centre() const
	{
		// TODO: caching
		
		// The local origin offset is given as an offset from the centre. Since the origin is
		// at (0,0) in local space, the centre in local space has the opposite offset as the
		// origin.

		m_Centre = as_transform().apply(-m_LocalOriginOffset);

		return m_Centre;
	}
		
	//-------------------------------------------------------------------------------------
		
	const Point& BoundingBox::origin() const
	{
		// The translation is applied to the origin, so the origin is just the translation
		return translation();
	}
		
	//-------------------------------------------------------------------------------------
		
	const glm::vec2& BoundingBox::size() const
	{
		return m_Size;
	}
		
	//-------------------------------------------------------------------------------------
		
	const QuadMesh& BoundingBox::mesh() const
	{
		//TODO: caching

		m_Mesh = QuadMesh::Create(as_transform(), m_Size, m_LocalOriginOffset);

		return m_Mesh;
	}
			
	//-------------------------------------------------------------------------------------
		
	bool BoundingBox::is_axis_alligned() const
	{
		//TODO: proper caching

		// The box is axis alligned if its rotation is a multiple of 90 degrees. 
		m_AxisAlligned = std::fabsf(std::fmodf(rotation_degrees(), 90.0f)) <= std::numeric_limits<float>::epsilon();

		return m_AxisAlligned;
	}
			
	//-------------------------------------------------------------------------------------
		
	BoundingBox BoundingBox::wrap_axis_alligned() const
	{
		// Simply create a new bounding box using the extents of this box
		return {this->extent()};
	}

	//-------------------------------------------------------------------------------------

}
