#include "Rectangle.hpp"

#include <glm/gtx/projection.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "../../Algorithms/SeparatingAxisTheorem.hpp"
#include "Mesh.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	glm::vec2 Rectangle::transform_to_local(const glm::vec2 point) const
	{
		// Transform a point to be in the local space of the rectangle, with their
		// coordinates specified along the axes of the rectangle. This is done by projecting
		// the vector from the centre of the rectangle to the point along the axes.

		const glm::vec2 centre_to_point = point - centre();
		const auto [horz_axis, vert_axis] = axes();

		// Note: since the axes are unit vectors, we can do the projection
		// by simply taking the dot product. 
		return {
			glm::dot(centre_to_point, horz_axis),
			glm::dot(centre_to_point, vert_axis)
		};
	}
	
	//-------------------------------------------------------------------------------------

	std::tuple<glm::vec2, glm::vec2> Rectangle::axes() const
	{
		// To get the axes of the rectangle, we just rotate the normal cartesian axes by the 
		// rotation of rectangle. Doing this will avoid having to generate the mesh for this operation.

		const float cos = std::cosf(rotation_radians());
		const float sin = std::sinf(rotation_radians());

		return {{cos, sin} ,{-sin, cos}};
	}
	
	//-------------------------------------------------------------------------------------

	Rectangle::Rectangle()
		: m_Size(0,0)
	{}
	
	//-------------------------------------------------------------------------------------
	
	Rectangle::Rectangle(const glm::vec2 size)
	{
		resize(size);
	}

	//-------------------------------------------------------------------------------------

	Rectangle::Rectangle(const Transform& transform, const glm::vec2 size)
	{
		resize(size);
		this->transform_to(transform);
	}
	
	//-------------------------------------------------------------------------------------
	
	void Rectangle::resize(const glm::vec2 size)
	{
		m_Size = glm::abs(size);
	}
	
	//-------------------------------------------------------------------------------------
	
	const glm::vec2& Rectangle::size() const
	{
		return m_Size;
	}
	
	//-------------------------------------------------------------------------------------
	
	const glm::vec2& Rectangle::centre() const
	{
		return translation();
	}
	
	//-------------------------------------------------------------------------------------
	
	StaticMesh<4> Rectangle::mesh() const
	{
		const auto& local_extent = Extent(m_Size);
		const auto max = local_extent.max();
		const auto min = local_extent.min();
		std::array<glm::vec2, 4> local_vertices{};

		local_vertices[0].x = min.x;
		local_vertices[0].y = max.y;

		local_vertices[1].x = min.x;
		local_vertices[1].y = min.y;

		local_vertices[2].x = max.x;
		local_vertices[2].y = min.y;

		local_vertices[3].x = max.x;
		local_vertices[3].y = max.y;

		return StaticMesh<4>{as_transform(), local_vertices};
	}
	
	//-------------------------------------------------------------------------------------
	
	Extent Rectangle::extent() const
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

		const auto& vertices = this->mesh().vertices();

		if(rotation >= 0 && rotation < 90)
		{
			// vertex 0 is top, vertex 1 is left, vertex 2 is bottom, vertex 3 is right
			max_x = vertices[3].x;
			min_x = vertices[1].x;
			max_y = vertices[0].y;
			min_y = vertices[2].y;
		}
		else if(rotation >= 90 && rotation < 180)
		{
			// vertex 0 is left, vertex 1 is bottom, vertex 2 is right, vertex 3 is top
			min_x = vertices[0].x;
			max_x = vertices[2].x;
			min_y = vertices[1].y;
			max_y = vertices[3].y;
		}
		else if(rotation >= 180 && rotation < 270)
		{
			// vertex 0 is bottom, vertex 1 is right, vertex 2 is top, vertex 3 is left
			max_x = vertices[1].x;
			min_x = vertices[3].x;
			max_y = vertices[2].y;
			min_y = vertices[0].y;
		}
		else if(rotation >= 270 && rotation < 360)
		{
			// vertex 0 is right, vertex 1 is top, vertex 2 is left, vertex 3 is bottom
			min_x = vertices[2].x;
			max_x = vertices[0].x;
			min_y = vertices[3].y;
			max_y = vertices[1].y;
		}
		else CBN_Assert(false, "The rotation is not wrapping between 0 and 360 as expected");

		return Extent{{min_x, min_y}, {max_x, max_y}};
	}
	
	//-------------------------------------------------------------------------------------

	template<>
	bool overlaps(const Rectangle& c1, const Rectangle& c2)
	{
		// We need to check for overlap using the separating axis theorem. 
		// However, since two sides of the rectangle are parallel, we can do 
		// it with half the axes as an optimisation. 

		const auto& mesh_1 = c1.mesh();
		const auto& mesh_2 = c2.mesh();

		const auto& normals_1 = mesh_1.normals();
		const auto& normals_2 = mesh_2.normals();

		const auto& vertices_1 = mesh_1.vertices();
		const auto& vertices_2 = mesh_2.vertices();

		const std::array<glm::vec2, 4> axes{
			normals_1[0], normals_1[3], normals_2[0], normals_2[3]
		};

		return test_separating_axis_theorem(
			std::span(axes),
			std::span(vertices_1),
			std::span(vertices_2)
		);
	}

	//-------------------------------------------------------------------------------------

	template<>
	bool overlaps(const Rectangle& c1, const Circle& c2)
	{
		// Transform the circle centre so that its position is given along the axes of the 
		// rectangle so that we can treat the rectangle as if it were an AABB. The overlap
		// test is then the same as the extent and circle one. 

		const auto local_centre_to_centre = c1.transform_to_local(c2.centre());

		const float radius = c2.radius();
		const glm::vec2 half_extent = c1.size() * 0.5f;
		const glm::vec2 closest_point = glm::clamp(local_centre_to_centre, -half_extent, half_extent);

		return glm::distance2(local_centre_to_centre, closest_point) <= radius * radius;
	}

	//-------------------------------------------------------------------------------------

	template<>
	bool overlaps(const Rectangle& c1, const Extent& c2)
	{
		// Re-use the overlap test from the extent's side
		return overlaps(c2, c1);
	}

	//-------------------------------------------------------------------------------------

	template<>
	bool overlaps(const Rectangle& c1, const Polygon& c2)
	{
		// We need to check for overlap using the separating axis theorem. Since the
		// rectangle has two parallel sides, we just need to add two of its normals 
		// to the polygon's normals for the SAT. 

		const auto& mesh_1 = c1.mesh();
		const auto& mesh_2 = c2.mesh();

		const auto& normals_1 = mesh_1.normals();
		const auto& normals_2 = mesh_2.normals();

		const auto& vertices_1 = mesh_1.vertices();
		const auto& vertices_2 = mesh_2.vertices();

		// Only use two of the normals since the sides are parallel
		const std::array<glm::vec2, 2> optimised_rect_normals {
			normals_1[0], normals_1[3]
		};

		return test_separating_axis_theorem(
			std::span(optimised_rect_normals.begin(), optimised_rect_normals.end()), 
			std::span(normals_2.begin(), normals_2.end()), 
			std::span(vertices_1.begin(), vertices_1.end()),
			std::span(vertices_2.begin(), vertices_2.end())
		);
	}

	//-------------------------------------------------------------------------------------

	template<>
	bool contains(const Rectangle& c, const glm::vec2& p)
	{
		// Transform the point to be in the local space of the rectangle then do a simple AABB test
		// with the rectangle's centre being at (0,0).
		const auto local_point = c.transform_to_local(p);
		const auto half_extent = c.size() * 0.5f;

		return local_point.x >= -half_extent.x
			&& local_point.x <=  half_extent.x
			&& local_point.y <=  half_extent.y
			&& local_point.y >= -half_extent.y;
	}

	//-------------------------------------------------------------------------------------
}