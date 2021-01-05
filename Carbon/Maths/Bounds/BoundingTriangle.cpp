#include "BoundingTriangle.hpp"

#include <algorithm>

//TODO: also normalize the scaling crap

#include "../Maths.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	void BoundingTriangle::update_min_max_coords()
	{
		// Since the triangle can be any possible triangle in any orientation, the only way to get 
		// its min and max coords is to manually find the minimum and maximum coords. Unlike with 
		// rectangles where we can use rectangle properties to speed this up. 

		const auto& vertices = this->mesh().vertices;

		const auto [min_x, max_x] = std::minmax_element(vertices, vertices + 4, [](const glm::vec2& a, const glm::vec2& b){
			return a.x < b.x;
		});
		const auto [min_y, max_y] = std::minmax_element(vertices, vertices + 4, [](const glm::vec2& a, const glm::vec2& b){
			return a.x < b.x;
		});

		m_MinMaxCoords = {{min_x->x, min_y->y}, {max_x->x, max_y->y}};
	}
	
	//-------------------------------------------------------------------------------------

	BoundingTriangle::BoundingTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& local_origin)
	{
		reshape(p1, p2, p3, local_origin);
	}

	//-------------------------------------------------------------------------------------

	BoundingTriangle::BoundingTriangle(const Transform& transform, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2 & local_origin)
		: m_Transform(transform)
	{
		// m_Transform is applied when we reshape, so theres no need to do it manually. 
		// Note that any scaling is handled in the reshape function
		reshape(p1, p2, p3, local_origin);
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
			box_mesh.normal_3, box_mesh.normal_4,
			tri_mesh.normal_1, tri_mesh.normal_2, tri_mesh.normal_3
		};

		return sat_test<5,4,3>(axes, box_mesh.vertices, tri_mesh.vertices);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::overlaps(const BoundingCircle& circle) const
	{
		// Simply check if the circle centre is inside the triangle or the circle
		// intersects one of the triangles' edges. This isn't great but circle-segment
		// intersection tests are pretty fast.

		const auto& mesh = this->mesh();

		return contains_point(circle.centre())
			|| circle.is_intersected_by_line_segment(mesh.vertex_1, mesh.vertex_2)
			|| circle.is_intersected_by_line_segment(mesh.vertex_2, mesh.vertex_3)
			|| circle.is_intersected_by_line_segment(mesh.vertex_3, mesh.vertex_1);
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

		return sat_test<6,3,3>(axes, mesh_1.vertices, mesh_2.vertices);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::encloses(const BoundingBox& box) const
	{
		// The box is enclosed in the triangle if all its vertices are inside
		const auto& mesh = box.mesh();
		
		return contains_point(mesh.vertex_1)
			&& contains_point(mesh.vertex_2)
			&& contains_point(mesh.vertex_3)
			&& contains_point(mesh.vertex_4);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::encloses(const BoundingCircle& circle) const
	{
		// The circle is enclosed if the centre point is inside the triangle
		// but the circle doesn't intersect any of the edges of the triangle
		const auto& mesh = this->mesh();

		return contains_point(circle.centre())
			&& !circle.is_intersected_by_line_segment(mesh.vertex_1, mesh.vertex_2)
			&& !circle.is_intersected_by_line_segment(mesh.vertex_2, mesh.vertex_3)
			&& !circle.is_intersected_by_line_segment(mesh.vertex_3, mesh.vertex_1);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::encloses(const BoundingTriangle& triangle) const
	{
		// The triangle is enclosed if all its vertices are inside
		const auto& mesh = triangle.mesh();
		
		return contains_point(mesh.vertex_1)
			&& contains_point(mesh.vertex_2)
			&& contains_point(mesh.vertex_3);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::contains_point(const glm::vec2& point) const
	{
		// Determine whether the point is inside the triangle by using barycentric coordinates
		const auto& mesh = this->mesh();
		
		const float s = mesh.vertex_1.y * mesh.vertex_3.x - mesh.vertex_1.x * mesh.vertex_3.y + (mesh.vertex_3.y - mesh.vertex_1.y) * point.x + (mesh.vertex_1.x - mesh.vertex_3.x) * point.y;
		const float t = mesh.vertex_1.x * mesh.vertex_2.y - mesh.vertex_1.y * mesh.vertex_2.x + (mesh.vertex_1.y - mesh.vertex_2.y) * point.x + (mesh.vertex_2.x - mesh.vertex_1.x) * point.y;
		const float a = -mesh.vertex_2.y * mesh.vertex_3.x + mesh.vertex_1.y * (-mesh.vertex_2.x + mesh.vertex_3.x) + mesh.vertex_1.x * (mesh.vertex_2.y - mesh.vertex_3.y) + mesh.vertex_2.x * mesh.vertex_3.y;
		
		return s > 0 && t > 0 && (s + t) < a;
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::is_intersected_by_line(const glm::vec2& p1, const glm::vec2& p2) const
	{
		// The triangle is intersected by the line if not all vertices lie on the same side of the line
		// Or one of the vertices is directly intersected by the line. 

		const auto& mesh = this->mesh();

		// The line side function returns +1 if the vertex is right of the line, -1 if its on 
		// its left and 0 if its on the line. So for the box to be intersected, at least one
		// vertex has to be left of the line, and at least one must be on its right. Or one
		// has to be on the line
		const auto s1 = line_side(p1, p2, mesh.vertex_1);
		const auto s2 = line_side(p1, p2, mesh.vertex_2);
		const auto s3 = line_side(p1, p2, mesh.vertex_3);

		return (s1 >= 0 || s2 >= 0 || s3 >= 0) && (s1 <= 0 || s2 <= 0 || s3 <= 0);
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::is_intersected_by_line_segment(const glm::vec2& p1, const glm::vec2& p2) const
	{
		// Simply check if the end points are in the triangle, or the line intersects any of the sides of the triangle

		// First to an infinite line intersection test as it is fast. If the 
		// infinte line doesnt intersect, than the segment definitely won't. 
		return is_intersected_by_line(p1, p2) 
			&& (contains_point(p1)
			|| contains_point(p2)
			|| segments_intersect(m_LocalVertex1, m_LocalVertex2, p1, p2)
			|| segments_intersect(m_LocalVertex2, m_LocalVertex3, p1, p2)
			|| segments_intersect(m_LocalVertex3, m_LocalVertex1, p1, p2));
	}

	//-------------------------------------------------------------------------------------

	bool BoundingTriangle::is_intersected_by_ray(const glm::vec2& origin, const glm::vec2& towards) const
	{
		// To test this, we will do an extended version of the line intersection test. We form an infinite
		// line from the ray, and an infinite line perpendicular to the ray which intersects the origin. 
		// For the triangle to intersect the infinite line formed by the ray, theres needs to be at least
		// one vertex on its left and at least one on its right. However, we want to restrict this intersection
		// to the ray, not the infinite line. So we also test that the vertices which intersect the line are
		// on the ray side of the perpendicular line which intersects the origin.
		//
		//	For example, given that (+, -) represents the plus side of the ray line
		//	and the negative side of the normal line. We need at least one (-, +) and
		//	at least one (+, +) as shown below. Note that the ray is vertical here. 
		//
		//				^
		//			-	|    +
		//				|
		//		(-, +)	|  (+, +)
		//				|
		//			+	|
		//		<-------*-------
		//			-
		//		(-,-)	   (+, -)
		
		const auto& mesh = this->mesh();

		const auto ray_s1 = line_side(origin, towards, mesh.vertex_1);
		const auto ray_s2 = line_side(origin, towards, mesh.vertex_2);
		const auto ray_s3 = line_side(origin, towards, mesh.vertex_3);

		// Calculate the perpendicular direction of the ray. The 
		// ray normal vector is obtained by a counter clockwise rotation
		// of 90 degrees from the ray direction, meaning that the normal
		// line has its right side towards the ray direction. 
		const glm::vec2 ray_dir = towards - origin;
		const glm::vec2 ray_normal = {-ray_dir.y, ray_dir.x};

		const float normal_s1 = line_side(origin, origin + ray_normal, mesh.vertex_1);
		const float normal_s2 = line_side(origin, origin + ray_normal, mesh.vertex_2);
		const float normal_s3 = line_side(origin, origin + ray_normal, mesh.vertex_3);

		const bool left_of_ray = (ray_s1 >= 0 && normal_s1 >= 0)
							  || (ray_s2 >= 0 && normal_s2 >= 0)
							  || (ray_s3 >= 0 && normal_s3 >= 0);

		const bool right_of_ray = (ray_s1 <= 0 && normal_s1 >= 0)
							   || (ray_s2 <= 0 && normal_s2 >= 0)
							   || (ray_s3 <= 0 && normal_s3 >= 0);

		return left_of_ray && right_of_ray;
	}

	//-------------------------------------------------------------------------------------

	void BoundingTriangle::reshape(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& local_origin)
	{
		// The shape is defined by the relative position of p1, p2 and p3. 
		// To make the local mesh, we first find the centroid of the triangle.
		// We do this by finding the average position of all vertices. 
		const glm::vec2 centroid = (p1 + p2 + p3) * (1.0f / 3.0f);
	
		// The origin is then an offset from the centroid
		const glm::vec2 origin = centroid + local_origin;

		// In local space, the origin is at (0,0) so to convert the 3 points
		// to local space, we want to use the offset between them and the origin 
		// in their current coordinate system.
		m_LocalVertex1 = p1 - origin;
		m_LocalVertex2 = p2 - origin;
		m_LocalVertex3 = p3 - origin;

		// The origin is defined by an offset from the centre. Since the local origin is 
		// at (0,0), then the local centre is defined by the opposite offset. 
		m_LocalCentre = -local_origin;

		// Since the local mesh changed, we need to re-apply the transform to update
		// the transformed mesh.
		transform_to(m_Transform);
	}

	//-------------------------------------------------------------------------------------

	void BoundingTriangle::transform_to(const Transform& transform)
	{
		// Remove the scaling as it is absorbed by the mesh. There
		// is no size metric for the triangle, so the scaling is 
		// only apparent on the mesh. 
		m_Transform = transform;
		m_Transform.scale_to(1);

		// Transform the mesh vertices and centre
		m_Mesh.vertex_1 = m_Transform.apply(m_LocalVertex1);
		m_Mesh.vertex_2 = m_Transform.apply(m_LocalVertex2);
		m_Mesh.vertex_3 = m_Transform.apply(m_LocalVertex3);
		m_Centre = m_Transform.apply(m_LocalCentre);

		// Update the unit normals of the triangle, these are obtained 
		// by a counter-clockwise rotation of each edge by 90 degrees;
		const auto unit_edge_1 = glm::normalize(m_Mesh.vertex_1 - m_Mesh.vertex_2);
		const auto unit_edge_2 = glm::normalize(m_Mesh.vertex_2 - m_Mesh.vertex_3);
		const auto unit_edge_3 = glm::normalize(m_Mesh.vertex_3 - m_Mesh.vertex_1);

		m_Mesh.normal_1.x = -unit_edge_1.y;
		m_Mesh.normal_1.y =  unit_edge_1.x;

		m_Mesh.normal_2.x = -unit_edge_2.y;
		m_Mesh.normal_2.y =  unit_edge_2.x;

		m_Mesh.normal_3.x = -unit_edge_3.y;
		m_Mesh.normal_3.y =  unit_edge_3.x;

		// Update the min and max coords now that the mesh has changed
		update_min_max_coords();
	}

	//-------------------------------------------------------------------------------------

	void BoundingTriangle::transform_by(const Transform& transform)
	{
		transform_to(transform.apply(m_Transform));
	}

	//-------------------------------------------------------------------------------------

	const glm::vec2& BoundingTriangle::centre() const
	{
		return m_Centre;
	}

	//-------------------------------------------------------------------------------------

	const glm::vec2& BoundingTriangle::origin() const
	{
		// The transforms are applied about the origin, so the 
		// origin is the same as the translation. 
		return m_Transform.translation();
	}

	//-------------------------------------------------------------------------------------

	const TriangleMesh& BoundingTriangle::mesh() const
	{
		return m_Mesh;
	}

	//-------------------------------------------------------------------------------------

	cbn::BoundingBox BoundingTriangle::wrap_axis_alligned() const
	{
		// The AABB's size is defined by the min and max coords.
		// The box is then centred with the centre of the triangle. 

		const auto& [min, max] = min_max_coords();
		return {Transform{centre()}, max - min};
	}
	
	//-------------------------------------------------------------------------------------

	const std::tuple<glm::vec2, glm::vec2>& BoundingTriangle::min_max_coords() const
	{
		return m_MinMaxCoords;
	}

	//-------------------------------------------------------------------------------------
}