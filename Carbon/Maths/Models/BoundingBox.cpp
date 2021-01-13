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

	bool BoundingBox::encloses(const Collider& collider) const
	{
		// For the collider to be enclosed, it needs to overlap with the box
		// but not come in contact with any of the boxes edges.
		
		const auto& mesh = this->mesh();
		return overlaps(collider)
			&& !collider.intersected_by(mesh.edge_1)
			&& !collider.intersected_by(mesh.edge_2)
			&& !collider.intersected_by(mesh.edge_3)
			&& !collider.intersected_by(mesh.edge_4);
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
		const auto old_offset = m_LocalOriginOffset;

		// The origin specifies the origin of the box as an offset from the centre of the box.
		// If the local_coords flag is set, then the offset is specified in the local space of 
		// the box. Otherwise its specified using world coordinates. 
		if(local_coords)
			m_LocalOriginOffset = origin_offset;
		else
		{
			// If the origin is not specified using local coordinates then we need 
			// to convert the origin offset to use local coordinates first. 
			// This is done by projecting the given offset across the two normals differing
			// positive normals of the box, which are alligned with the boxes local axes.
			const auto& mesh = this->mesh();

			// Note that since the normals of the both are unit vectors, we can do 
			// the projection with a simple dot product.
			m_LocalOriginOffset.x = glm::dot(origin_offset, mesh.normal_4);
			m_LocalOriginOffset.y = glm::dot(origin_offset, mesh.normal_1);
		}

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

}

//	//-------------------------------------------------------------------------------------
//
//	void BoundingBox::update_min_max_coords()
//	{
//		// To find the minimum and maximum x and y coords, we can just make use of the 
//		// rotation of the bounding box along with the properties of a rectangle. Depending on which 
//		// quadrant the box is facing towards, we can instantly determine which vertex of the box 
//		// will be the topmost, leftmost, rightmost and bottommost. This is possible because the
//		// sides of the rectangle are either perpendicular or parallel to each other. 
//
//		// Note that the rotation goes counter clockwise, so a 90 degree rotation is 90 degrees counter clockwise
//		const auto rotation = m_Transform.rotation_degrees();
//
//		float min_x = 0, max_x = 0, min_y = 0, max_y = 0;
//
//		if(rotation >= 0 && rotation < 90)
//		{
//			// vertex 1 is top, vertex 2 is left, vertex 3 is bottom, vertex 4 is right
//			max_x = m_Mesh.vertex_4.x;
//			min_x = m_Mesh.vertex_2.x;
//			max_y = m_Mesh.vertex_1.y;
//			min_y = m_Mesh.vertex_3.y;
//		}
//		else if(rotation >= 90 && rotation < 180)
//		{
//			// vertex 1 is left, vertex 2 is bottom, vertex 3 is right, vertex 4 is top
//			min_x = m_Mesh.vertex_1.x;
//			max_x = m_Mesh.vertex_3.x;
//			min_y = m_Mesh.vertex_2.y;
//			max_y = m_Mesh.vertex_4.y;
//		}
//		else if(rotation >= 180 && rotation < 270)
//		{
//			// vertex 1 is bottom, vertex 2 is right, vertex 3 is top, vertex 4 is left
//			max_x = m_Mesh.vertex_2.x;
//			min_x = m_Mesh.vertex_4.x;
//			max_y = m_Mesh.vertex_3.y;
//			min_y = m_Mesh.vertex_1.y;
//		}
//		else if(rotation >= 270 && rotation < 360)
//		{
//			// vertex 1 is right, vertex 2 is top, vertex 3 is left, vertex 4 is bottom
//			min_x = m_Mesh.vertex_3.x;
//			max_x = m_Mesh.vertex_1.x;
//			min_y = m_Mesh.vertex_4.y;
//			max_y = m_Mesh.vertex_2.y;
//		}
//		else CBN_Assert(false, "The rotation is not wrapping between 0 and 360 as expected");
//
//		m_MinMaxCoords = {{min_x, min_y}, {max_x, max_y}};
//	}
//	
//	//-------------------------------------------------------------------------------------
//
//	glm::vec2 BoundingBox::transform_to_local_aabb_space(const glm::vec2& point) const
//	{
//		// Transform a point to be in the local coordinate system of the box, where the box is AABB.
//		// This works because the point has no orientation, hence we can effectly transform the coordinate 
//		// system to allign it with the rotated box and treat it as AABB. However, in this case we do the 
//		// opposite and transform the point as if it were the boxes' local coordinate system. 
//		
//		// To do this, find the vector between the box origin and point and project it
//		// across two perpendicular normals of the box. This will give us the offset from 
//		// the boxes' origin to the point along these normals. The normals are alligned
//		// with the boxes edges so, if we get the offset along the normals we get the
//		// offset along the coordinate system whose axes are alligned with the boxes.
//		// We can then use this to transform the point to be in the boxes local 
//		// coordinate system where the box is AABB.
//
//		const auto& mesh = this->mesh();
//		const glm::vec2 origin_to_point = point - origin();
//
//		// Get the scalar projection of the origin to point vector across two
//		// perpendicular normals. Since the normals are unit vectors, we can
//		// simplify the projection to a dot product, avoiding any square roots.
//		return {glm::dot(mesh.normal_3, origin_to_point), glm::dot(mesh.normal_4, origin_to_point)};
//	} 
//
//	//-------------------------------------------------------------------------------------
//
//	BoundingBox::BoundingBox(const glm::vec2& size, const glm::vec2& local_origin)
//	{
//		reshape(size, local_origin);
//	}
//	
//	//-------------------------------------------------------------------------------------
//
//	BoundingBox::BoundingBox(const Transform& transform, const glm::vec2& size, const glm::vec2& local_origin)
//		: m_Transform(transform)
//	{
//		// Any scaling on the transform should be absorbed by the size, so remove it and apply it manually
//		// to the size. Note that when we initialize the local mesh, the m_Transform transform is automatically
//		// applied to the mesh as the reshape method is called.
//		m_Transform.scale_to(1);
//		reshape(size, local_origin);
//	}
//	
//	//-------------------------------------------------------------------------------------
//
//	bool BoundingBox::overlaps(const BoundingBox& box, const float axis_snap_threshold) const
//	{
//		// If both boxes are axis alligned, or are within the threshold to be 
//		// snapped to the axes, then perform a quick AABB overlap calcuation.
//		// Otherwise, we need to do SAT which is slower. 
//
//		const float allignment_error_1 = std::fabsf(std::fmodf(m_Transform.rotation_degrees(), 90.0f));
//		const float allignment_error_2 = std::fabsf(std::fmodf(box.m_Transform.rotation_degrees(), 90.0f));
//
//		if((is_axis_alligned() || allignment_error_1 <= axis_snap_threshold) && (box.is_axis_alligned() || allignment_error_2 <= axis_snap_threshold))
//		{
//			// If we are here, we can treat both boxes as AABB
//
//			const auto& [min_1, max_1] = min_max_coords();
//			const auto& [min_2, max_2] = box.min_max_coords();
//
//			// The boxes overlap if their min/max coords overlap on both axes. 
//			return (max_1.x >= min_2.x) && (min_1.x <= max_2.x)
//				&& (max_1.y >= min_2.y) && (min_1.y <= max_2.y);
//		}
//		else
//		{
//			// If we are here, we have to use the separating axis theorem on both boxes
//			// Note that since boxes have two parallel sides, we only have to check 2 axes. 
//			// Which should be a lot faster than the normal SAT which would check 4. 
//
//			const auto& mesh_1 = mesh();
//			const auto& mesh_2 = box.mesh();
//
//			// Get the axes of the boxes by just using two of their differing normals. 
//			const std::array<glm::vec2, 4> axes = {
//				mesh_1.normal_3, mesh_1.normal_4, 
//				mesh_2.normal_3, mesh_2.normal_4,
//			};
//
//			return sat_test<4, 4, 4>(axes, mesh_1.vertices, mesh_2.vertices);
//		}
//	}
//	
//	//-------------------------------------------------------------------------------------
//
//	bool BoundingBox::overlaps(const BoundingCircle& circle) const
//	{
//		// The circle has no orientation, so we can transform its centre to be
//		// in the local mesh space, and then treat this as an AABB to circle 
//		// collision. We need to do this in the box class as we don't have
//		// access to the local mesh from the circle class. 
//
//		const glm::vec2 local_circle_centre = transform_to_local_aabb_space(circle.centre());
//
//		// The circle to AABB collision test is done by finding the closest point in
//		// the box to the circle and seeing if the distance between that point and
//		// the circle's centre is less than its radius. We find the closest point by
//		// getting a vector from the box centre to the circle centre and clamping
//		// its length by the half extent of the box. This effectively clamps the 
//		// vector so that it ends within the rectangle, hence is the closest point
//		// that is still inside the rectangle. 
//
//		const float radius = circle.radius();
//		const glm::vec2 box_half_size = size() * 0.5f;
//		const glm::vec2 closest_box_point = m_LocalCentre + glm::clamp(local_circle_centre - m_LocalCentre, -box_half_size, box_half_size);
//
//		return glm::distance2(local_circle_centre, closest_box_point) <= radius * radius;
//	}
//	
//	//-------------------------------------------------------------------------------------
//
//	bool BoundingBox::overlaps(const BoundingTriangle& triangle) const
//	{
//		// This uses more triangle maths, so do it from the triangle's side
//		return triangle.overlaps(*this);
//	}
//	
//	//-------------------------------------------------------------------------------------
//
//	bool BoundingBox::encloses(const BoundingBox& box) const
//	{
//		// The box is enclosed if all its points are within this box
//		const auto& mesh = box.mesh();
//		return contains_point(mesh.vertex_1) 
//			&& contains_point(mesh.vertex_2) 
//			&& contains_point(mesh.vertex_3)
//			&& contains_point(mesh.vertex_4);
//	}
//	
//	//-------------------------------------------------------------------------------------
//
//	bool BoundingBox::encloses(const BoundingCircle& circle) const
//	{
//		// The box encloses the circle if the circle's centre is inside the box, 
//		// and the circle is not intersected by any of the box's edges. 
//
//		// Note: This can probably be optimised mathematically if it becomes a problem.
//		const auto& mesh = this->mesh();
//		return contains_point(circle.centre())
//			&& !circle.is_intersected_by_line_segment(mesh.vertex_1, mesh.vertex_2)
//			&& !circle.is_intersected_by_line_segment(mesh.vertex_2, mesh.vertex_3)
//			&& !circle.is_intersected_by_line_segment(mesh.vertex_3, mesh.vertex_4)
//			&& !circle.is_intersected_by_line_segment(mesh.vertex_4, mesh.vertex_1);
//	}
//
//	//-------------------------------------------------------------------------------------
//
//	bool BoundingBox::encloses(const BoundingTriangle& triangle) const
//	{
//		// The box encloses the triangle if all its vertices are inside the box
//		const auto& mesh = triangle.mesh();
//		return contains_point(mesh.vertex_1) 
//			&& contains_point(mesh.vertex_2)
//			&& contains_point(mesh.vertex_3);
//	}
//
//	//-------------------------------------------------------------------------------------
//
//	bool BoundingBox::contains_point(const glm::vec2& point) const
//	{
//		const auto local_point = transform_to_local_aabb_space(point);
//
//		// We can now do a simple aabb check.
//		return local_point.x >= m_LocalVertex1.x
//			&& local_point.x <= m_LocalVertex4.x
//			&& local_point.y <= m_LocalVertex1.y
//			&& local_point.y >= m_LocalVertex2.y;
//	}
//
//	//-------------------------------------------------------------------------------------
//
//	bool BoundingBox::is_intersected_by_line(const glm::vec2& p1, const glm::vec2& p2) const
//	{
//		// The box is intersected by the line if not all vertices lie on the same side of the line
//		// Or one of the vertices is directly intersected by the line. 
//
//		const auto& mesh = this->mesh();
//
//		// The line side function returns +1 if the vertex is right of the line, -1 if its on 
//		// its left and 0 if its on the line. So for the box to be intersected, at least one
//		// vertex has to be left of the line, and at least one must be on its right. Or one
//		const auto s1 = line_side(p1, p2, mesh.vertex_1);
//		const auto s2 = line_side(p1, p2, mesh.vertex_2);
//		const auto s3 = line_side(p1, p2, mesh.vertex_3);
//		const auto s4 = line_side(p1, p2, mesh.vertex_4);
//
//		return (s1 >= 0 || s2 >= 0 || s3 >= 0 || s4 >= 0) && (s1 <= 0 || s2 <= 0 || s3 <= 0 || s4 <= 0);
//	}
//
//	//-------------------------------------------------------------------------------------
//
//	bool BoundingBox::is_intersected_by_line_segment(const glm::vec2& p1, const glm::vec2& p2) const
//	{
//		// First check if the infinite line formed by p1 and p2 intersects the box. If the infinite 
//		// line doesn't intersect, then the segment definitely doesn't. If it does intersect, we need
//		// to ensure that the intersection point lies on the segment. We do this by checking if the
//		// line overlaps both axes of the box. Since it has to overlap the axes of the both, we 
//		// transform the line to be in the boxes local space where the box is AABB in order to 
//		// make the calculation easier. 
//
//		const auto local_p1 = transform_to_local_aabb_space(p1);
//		const auto local_p2 = transform_to_local_aabb_space(p2);
//
//		const auto [min_x, max_x] = std::minmax(local_p1.x, local_p2.x);
//		const auto [min_y, max_y] = std::minmax(local_p1.y, local_p2.y);
//	
//		return is_intersected_by_line(p1, p2)
//			&& (max_x >= m_LocalVertex1.x)
//			&& (min_x <= m_LocalVertex4.x)
//			&& (max_y >= m_LocalVertex2.y)
//			&& (min_y <= m_LocalVertex1.y);
//	}
//
//	//-------------------------------------------------------------------------------------
//
//	bool BoundingBox::is_intersected_by_ray(const glm::vec2& origin, const glm::vec2& towards) const
//	{
//		// To test this we first transform the ray to bein local coordinates so that the box is AABB.
//		// Then we perform the simple branchless slab test. 
//
//		const auto local_ray_origin = transform_to_local_aabb_space(origin);
//		const auto local_ray_towards = transform_to_local_aabb_space(towards);
//		const auto local_ray_dir = glm::normalize(local_ray_towards - local_ray_origin);
//
//		const float inverse_dir_x = 1.0f / local_ray_dir.x;
//		const float inverse_dir_y = 1.0f / local_ray_dir.y;
//
//		// To do the slab test, we want to use the minimum and maximum coordinates
//		// of the AABB. For the local box, this is vertex 2 and 4 respectively. 
//
//		float t_x1 = (m_LocalVertex2.x - local_ray_origin.x) * inverse_dir_x;
//		float t_x2 = (m_LocalVertex4.x - local_ray_origin.x) * inverse_dir_x;
//
//		float t_y1 = (m_LocalVertex2.y - local_ray_origin.y) * inverse_dir_y;
//		float t_y2 = (m_LocalVertex4.y - local_ray_origin.y) * inverse_dir_y;
//
//		const auto [min_x, max_x] = std::minmax(t_x1, t_x2);
//		const auto [min_y, max_y] = std::minmax(t_y1, t_y2);
//
//		const auto t_max = std::min(max_x, max_y);
//		const auto t_min = std::max(min_x, min_y);
//
//		return t_max >= 0 && t_min <= t_max;
//	}
//
//	//-------------------------------------------------------------------------------------
//
//	void BoundingBox::reshape(const glm::vec2& size, const glm::vec2& local_origin)
//	{
//		m_LocalOriginOffset = local_origin;
//		m_Size = size;
//
//		// Re-create the local mesh to match the new shape. The local origin is
//		// an from the centre of the local box which specifies the box's origin.
//		// The coordinate (0,0) of the local mesh is the origin of the box, so we 
//		// need to offset all the vertices so the origin of the box has the given 
//		// offset from the centre while still being at (0,0).
//		
//		const glm::vec2 half_size = size * 0.5f;
//
//		// Top left vertex
//		m_LocalVertex1.x = -half_size.x - local_origin.x;
//		m_LocalVertex1.y =  half_size.y - local_origin.y;
//		
//		// Bottom left vertex
//		m_LocalVertex2.x = -half_size.x - local_origin.x;
//		m_LocalVertex2.y = -half_size.y - local_origin.y;
//
//		// Bottom right vertex
//		m_LocalVertex3.x =  half_size.x - local_origin.x;
//		m_LocalVertex3.y = -half_size.y - local_origin.y;
//
//		// Top right vertex
//		m_LocalVertex4.x =  half_size.x - local_origin.x;
//		m_LocalVertex4.y =  half_size.y - local_origin.y;
//	
//		// If the local origin is specified by an offset form the centre of the box, then 
//		// the centre of the box will have the opposite offset from the new origin.
//		m_LocalCentre = -local_origin;
//
//		// NEW
//		m_Transform.translate_by(local_origin);
//		transform_to(m_Transform);
//
//		/* OLD:
//			// With the local mesh changed, we need to update the transformed mesh.
//		// Note: the transform method depends on this being called here, so think 
//		// twice before removing it. 
//		transform_to(m_Transform);
//		
//		*/
//		
//	}
//	
//	//-------------------------------------------------------------------------------------
//
//	void BoundingBox::specify_origin(const glm::vec2& origin_offset, const bool use_local)
//	{}
//
//	//-------------------------------------------------------------------------------------
//
//	void BoundingBox::transform_to(const Transform& transform)
//	{
//		// Update the transform, and forcefully remove any scaling
//		m_Transform = transform;
//		m_Transform.scale_to(1);
//
//		// Apply the transform to the local mesh, and local centre 
//		m_Mesh.vertex_1 = m_Transform.apply(m_LocalVertex1);
//		m_Mesh.vertex_2 = m_Transform.apply(m_LocalVertex2);
//		m_Mesh.vertex_3 = m_Transform.apply(m_LocalVertex3);
//		m_Mesh.vertex_4 = m_Transform.apply(m_LocalVertex4);
//		m_Centre = m_Transform.apply(m_LocalCentre);
//
//		// Update the normals of the mesh, its simply a counter clockwise 90 degree rotation of each edge
//		// Since edges 3 and 4 are parallel to edges 1 and 2 respectively, we can get their normals just by 
//		// inverting normals 1 and 2. 
//
//		const auto unit_edge_1 = glm::normalize(m_Mesh.vertex_1 - m_Mesh.vertex_2);
//		m_Mesh.normal_1.x = -unit_edge_1.y;
//		m_Mesh.normal_1.y =  unit_edge_1.x;
//
//		m_Mesh.normal_3 = -m_Mesh.normal_1;
//		
//		const auto unit_edge_2 = glm::normalize(m_Mesh.vertex_2 - m_Mesh.vertex_3);
//		m_Mesh.normal_2.x = -unit_edge_2.y;
//		m_Mesh.normal_2.y =  unit_edge_2.x;
//
//		m_Mesh.normal_4 = -m_Mesh.normal_2;
//
//		// Determine if the box is axis alligned or not. This is done by seeing
//		// if the rotation of the box is a multiple of 90 degrees, which would 
//		// make it alligned to the axes. 
//		m_AxisAlligned = std::fabsf(std::fmodf(m_Transform.rotation_degrees(), 90.0f)) <= std::numeric_limits<float>::epsilon();
//
//		// Update the min max coords as the mesh and position of 
//		// the vertices have probably changed. 
//		update_min_max_coords();
//	}
//	
//	//-------------------------------------------------------------------------------------
//
//	void BoundingBox::transform_by(const Transform& transform)
//	{
//		transform_to(transform.apply(m_Transform));
//	}
//
//	//-------------------------------------------------------------------------------------
//
//	const glm::vec2& BoundingBox::centre() const
//	{
//		return m_Centre;
//	}
//
//	//-------------------------------------------------------------------------------------
//
//	const glm::vec2& BoundingBox::origin() const
//	{
//		// The translation is applied to the origin, so the origin is just the translation
//		return m_Transform.translation();
//	}
//
//	//-------------------------------------------------------------------------------------
//
//	const glm::vec2& BoundingBox::size() const
//	{
//		return m_Size;
//	}
//
//	//-------------------------------------------------------------------------------------
//
//	const cbn::BoxMesh& BoundingBox::mesh() const
//	{
//		return m_Mesh;
//	}
//	
//	//-------------------------------------------------------------------------------------
//
//	bool BoundingBox::is_axis_alligned() const
//	{
//		return m_AxisAlligned;
//	}
//	
//	//-------------------------------------------------------------------------------------
//
//	BoundingBox BoundingBox::wrap_axis_alligned() const
//	{
//		// Simply get the min max coords of the box, and use them to define the size 
//		// of the axis alligned box. The new box will be translated to the centre of the
//		// current box. The AABB won't preserve the origin, as the shape is now different.
//
//		const auto& [min, max] = min_max_coords();
//		return {Transform{centre()}, max - min};
//	}
//	
//	//-------------------------------------------------------------------------------------
//
//	const std::tuple<glm::vec2, glm::vec2>& BoundingBox::min_max_coords() const
//	{
//		return m_MinMaxCoords;
//	}
//
//	//-------------------------------------------------------------------------------------
//
//}
