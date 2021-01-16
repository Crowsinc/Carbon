#include "BoundingTriangle.hpp"

#include <algorithm>

#include "../Maths.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	void BoundingTriangle::update_extent() const
	{
		// Since the triangle can be any possible triangle in any orientation, the only way to get 
		// its min and max coords is to manually find the minimum and maximum coords. Unlike with 
		// rectangles where we can use rectangle properties to speed this up. 

		const auto& vertices = this->mesh().vertices;

		const auto [min_x, max_x] = std::minmax_element(vertices.begin(), vertices.end(), [](const glm::vec2& a, const glm::vec2& b){
			return a.x < b.x;
		});
		const auto [min_y, max_y] = std::minmax_element(vertices.begin(), vertices.end(), [](const glm::vec2& a, const glm::vec2& b){
			return a.y < b.y;
		});

		m_Extent.max = {max_x->x, max_y->y};
		m_Extent.min = {min_x->x, min_y->y};
	}
	
	//-------------------------------------------------------------------------------------

	BoundingTriangle::BoundingTriangle(const std::array<Point, TriangleMesh::Sides>& vertices)
		: m_LocalOriginOffset(0,0)
	{
		reshape(vertices);
	}

	//-------------------------------------------------------------------------------------

	BoundingTriangle::BoundingTriangle(const Transform& transform, const std::array<Point, TriangleMesh::Sides>& vertices)
		: Transformable(transform),
		  m_LocalOriginOffset(0, 0)

	{
		reshape(vertices);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::overlaps(const Collider& collider) const
	{
		// Just use the triangle overlap method of the collider
		return collider.overlaps(*this);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::overlaps(const BoundingBox& box) const
	{
		// This is done using SAT. For the triangle we need to check
		// all of its normals. However, since the box has two parallel
		// sides, we can just use two differing normals. 

		const auto& tri_mesh = mesh();
		const auto& box_mesh = box.mesh();

		const std::array<glm::vec2, 5> axes = {
			box_mesh.normal_1, box_mesh.normal_4,
			tri_mesh.normal_1, tri_mesh.normal_2, tri_mesh.normal_3
		};

		return sat_test(axes, box_mesh.vertices, tri_mesh.vertices);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::overlaps(const BoundingCircle& circle) const
	{
		// Simply check if the circle centre is inside the triangle or the circle
		// intersects one of the triangles' edges. This isn't great but circle-segment
		// intersection tests are pretty fast.

		const auto& mesh = this->mesh();

		return contains(circle.centre())
			|| circle.intersected_by(mesh.edge_1)
			|| circle.intersected_by(mesh.edge_2)
			|| circle.intersected_by(mesh.edge_3);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::overlaps(const BoundingTriangle& triangle) const
	{
		// Simply perform a SAT test
		const auto& mesh_1 = mesh();
		const auto& mesh_2 = triangle.mesh();

		const std::array<glm::vec2, 6> axes = {
			mesh_1.normal_1, mesh_1.normal_2, mesh_1.normal_3,
			mesh_2.normal_1, mesh_2.normal_2, mesh_2.normal_3
		};

		//TODO: switch to general polygon sat test
		return sat_test(axes, mesh_1.vertices, mesh_2.vertices);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::enclosed_by(const Collider& collider) const
	{
		return collider.encloses(*this);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::encloses(const BoundingBox& box) const
	{
		// The box is enclosed in the triangle if all its vertices are inside
		
		const auto& mesh = box.mesh();
		return contains(mesh.vertex_1)
			&& contains(mesh.vertex_2)
			&& contains(mesh.vertex_3)
			&& contains(mesh.vertex_4);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::encloses(const BoundingCircle& circle) const
	{
		// The circle is enclosed if the centre point is inside the triangle
		// and the circle doesn't intersect any of the edges of the triangle
		
		const auto& mesh = this->mesh();
		return contains(circle.centre())
			&& !circle.intersected_by(mesh.edge_1)
			&& !circle.intersected_by(mesh.edge_2)
			&& !circle.intersected_by(mesh.edge_3);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::encloses(const BoundingTriangle& triangle) const
	{
		// The triangle is enclosed if all its vertices are inside
		const auto& mesh = triangle.mesh();
		return contains(mesh.vertex_1)
			&& contains(mesh.vertex_2)
			&& contains(mesh.vertex_3);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::contains(const Point& point) const
	{
		// Determine whether the point is inside the triangle by using barycentric coordinates
		
		const auto& mesh = this->mesh();
		const float s =  mesh.vertex_1.y * mesh.vertex_3.x - mesh.vertex_1.x * mesh.vertex_3.y + (mesh.vertex_3.y - mesh.vertex_1.y) * point.x + (mesh.vertex_1.x - mesh.vertex_3.x) * point.y;
		const float t =  mesh.vertex_1.x * mesh.vertex_2.y - mesh.vertex_1.y * mesh.vertex_2.x + (mesh.vertex_1.y - mesh.vertex_2.y) * point.x + (mesh.vertex_2.x - mesh.vertex_1.x) * point.y;
		const float a = -mesh.vertex_2.y * mesh.vertex_3.x + mesh.vertex_1.y * (-mesh.vertex_2.x + mesh.vertex_3.x) + mesh.vertex_1.x * (mesh.vertex_2.y - mesh.vertex_3.y) + mesh.vertex_2.x * mesh.vertex_3.y;
		
		return s > 0 && t > 0 && (s + t) < a;
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::intersected_by(const Line& line) const
	{
		// The triangle is intersected by the line if not all vertices lie on the same side of the line
		// Or one of the vertices is directly intersected by the line. 

		const auto& mesh = this->mesh();

		// The line side function returns +1 if the vertex is right of the line, -1 if its on 
		// its left and 0 if its on the line. So for the box to be intersected, at least one
		// vertex has to be left of the line, and at least one must be on its right. Or one
		// has to be on the line
		const auto s1 = line_side(line.p1, line.p2, mesh.vertex_1);
		const auto s2 = line_side(line.p1, line.p2, mesh.vertex_2);
		const auto s3 = line_side(line.p1, line.p2, mesh.vertex_3);

		return (s1 >= 0 || s2 >= 0 || s3 >= 0) && (s1 <= 0 || s2 <= 0 || s3 <= 0);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::intersected_by(const Segment& segment) const
	{
		// Simply check if the end points are in the triangle, or the line intersects 
		// any of the sides of the triangle. Theres probably a faster way to do this
		const auto& mesh = this->mesh();
		
		return contains(segment.p1) || contains(segment.p2)
			|| segments_intersect(mesh.vertex_1, mesh.vertex_2, segment.p1, segment.p2)
			|| segments_intersect(mesh.vertex_2, mesh.vertex_3, segment.p1, segment.p2)
			|| segments_intersect(mesh.vertex_3, mesh.vertex_1, segment.p1, segment.p2);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::intersected_by(const Ray& ray) const
	{
		// Just check if the ray intersects any of the triangle edges. This is not very fast. 

		const auto& mesh = this->mesh();
		const auto ray_point = ray.origin + ray.direction;

		return ray_segment_intersection(ray.origin, ray_point, mesh.vertex_1, mesh.vertex_2)
		    || ray_segment_intersection(ray.origin, ray_point, mesh.vertex_2, mesh.vertex_3)
		    || ray_segment_intersection(ray.origin, ray_point, mesh.vertex_3, mesh.vertex_1);
	}

	//-------------------------------------------------------------------------------------

	const Extent& BoundingTriangle::extent() const
	{
		//TODO: caching
		
		update_extent();

		return m_Extent;
	}

	//-------------------------------------------------------------------------------------

	void BoundingTriangle::reshape(const std::array<Point, TriangleMesh::Sides>& vertices)
	{
		// The shape is defined by the relative position of p1, p2 and p3. 
		// To make the local mesh, we first find the centroid of the triangle.
		// We do this by finding the average position of all vertices. 
		const glm::vec2 centroid = (vertices[0] + vertices[1] + vertices[2]) * (1.0f / 3.0f);
	
		// In local space the origin is at (0,0). We want to convert the vertices
		// over to local space by finding their offset from their centroid, then offsetting
		// them using the local origin offset. That is, we find the vertices with the centroid
		// being (0,0), then add the origin offset to make the orign (0,0) instead. 
		m_LocalVertices[0] = (vertices[0] - centroid) - m_LocalOriginOffset;
		m_LocalVertices[1] = (vertices[1] - centroid) - m_LocalOriginOffset;
		m_LocalVertices[2] = (vertices[2] - centroid) - m_LocalOriginOffset;

		// TODO: caching
		// The mesh will need updating, so will the extents etc.
		m_Mesh = TriangleMesh::Create(as_transform(), m_LocalVertices);
	}

	//-------------------------------------------------------------------------------------

	void BoundingTriangle::specify_origin(const glm::vec2& origin_offset, const bool local_coords)
	{
		const Point old_centre = centre();

		// The origin specifies the origin of the box as an offset from the centre of the triangle.
		// If the local_coords flag is set, then the offset is specified in the local space of 
		// the triangle. Otherwise its specified using world coordinates. 
		if(!local_coords)
		{
			// If the offset is given in world coordinates, then we need to 
			// project the offset along the local axes of the triangle to turn 
			// them into local coordiantes. Since the normals direction vector
			// and its normal line up with the axes and are unit normals, we can 
			// do this with just two dot products. 
			const auto& dir = direction();
			const glm::vec2 dir_normal{dir.y, -dir.x};

			m_LocalOriginOffset.x = glm::dot(origin_offset, dir_normal);
			m_LocalOriginOffset.y = glm::dot(origin_offset, dir);
		}
		else m_LocalOriginOffset = origin_offset;

		// The translation is taken from the origin, so if the origin is changed then the triangle 
		// would move so that the new origin is placed (in world coordinates) where the old 
		// origin was. For this reason, we need to offset the translation so that the triangle stays 
		// with the same location in the world. The translation amount has to be obtained from the difference 
		// in position after transformation (non-local). The easiest way to do this is to observe the
		// difference in the transformed centre after the origin change, and translate the triangle by 
		// the opposite change to cancel it out. 
		translate_by(old_centre - centre());

		// The local mesh will need to be changed, so reshape the triangle to reform it
		reshape(m_LocalVertices);
	}
	
	//-------------------------------------------------------------------------------------

	const glm::vec2& BoundingTriangle::direction() const
	{
		// The local direction is from the centre upwards one unit
		// So specify the local direction as a point that is one unit 
		// above the local centre, then transform it and find the new direction
		// between this point and the transformed centre. Keep in mind that the 
		// local centre offset is the opposite offset as the origin.
		constexpr glm::vec2 unit_j{0.0f, 1.0f};
		m_Direction = as_transform().apply(-m_LocalOriginOffset + unit_j) - centre();

		return m_Direction;
	}

	//-------------------------------------------------------------------------------------

	const glm::vec2& BoundingTriangle::centre() const
	{
		//TODO: caching
		// The local origin is specified as an offset from the centroid, hence
		// the centroid can be obtained by the opposite offset of the origin.
		m_Centre = as_transform().apply(-m_LocalOriginOffset);

		return m_Centre;
	}

	//-------------------------------------------------------------------------------------

	const glm::vec2& BoundingTriangle::origin() const
	{
		// The transforms are applied about the origin, so the 
		// origin is the same as the translation. 
		return translation();
	}

	//-------------------------------------------------------------------------------------

	const TriangleMesh& BoundingTriangle::mesh() const
	{
		//TODO: caching
		m_Mesh = TriangleMesh::Create(as_transform(), m_LocalVertices);

		return m_Mesh;
	}

	//-------------------------------------------------------------------------------------

	cbn::BoundingBox BoundingTriangle::wrap_axis_alligned() const
	{
		// Simply create a bounding box from the extent
		return {extent()};
	}
	
	//-------------------------------------------------------------------------------------
}