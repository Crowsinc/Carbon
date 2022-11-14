#include "Extent.hpp"

#include <glm/gtx/projection.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "../../Algorithms/SeparatingAxisTheorem.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	Extent::Extent()
		: m_HalfSize(0, 0)
    {}

	//-------------------------------------------------------------------------------------

	Extent::Extent(const glm::vec2 size)
    {
		resize(size);
	}
	
	//-------------------------------------------------------------------------------------

	Extent::Extent(const glm::vec2 min, const glm::vec2 max)
	{
		reshape(min, max);
	}
	
	//-------------------------------------------------------------------------------------

	glm::vec2 Extent::min() const
    {
		return centre() - m_HalfSize;
    }
	
	//-------------------------------------------------------------------------------------

	glm::vec2 Extent::max() const
    {
		return centre() + m_HalfSize;
    }

	//-------------------------------------------------------------------------------------

	void Extent::resize(const glm::vec2 size)
    {
		m_HalfSize = size * 0.5f;
	}
	
	//-------------------------------------------------------------------------------------

	void Extent::reshape(const glm::vec2 min, const glm::vec2 max)
    {
		CBN_Assert(max.x > min.x && max.y > min.y, "Incorrect extents");

		// Update half-size and translation to match the new shape
		m_HalfSize = (max - min) * 0.5f;
		translate_to((max + min) * 0.5f);
	}
	
	//-------------------------------------------------------------------------------------

	glm::vec2 Extent::size() const
	{
		return m_HalfSize * 2.0f;
	}
	
	//-------------------------------------------------------------------------------------

	const glm::vec2& Extent::centre() const
	{
		return translation();
	}
	
	//-------------------------------------------------------------------------------------

	StaticMesh<4> Extent::mesh() const
	{
		// Form a quad from the all the corner points of the extent. 
		// Note that by convention, quads should be laid with the vertices as shown:
		/*
			  Vertices

			0----------3
			|		   |
			|          |
			|          |
			1----------2

		*/

		const auto& max = this->max();
		const auto& min = this->min();
		std::array<glm::vec2, 4> vertices{};

		vertices[0].x = min.x;
		vertices[0].y = max.y;
		
		vertices[1].x = min.x;
		vertices[1].y = min.y;
		
		vertices[2].x = max.x;
		vertices[2].y = min.y;

		vertices[3].x = max.x;
		vertices[3].y = max.y;

		return StaticMesh<4>(vertices);
	}

	//-------------------------------------------------------------------------------------

	template<>
	bool overlaps(const Extent& c1, const Extent& c2)
	{
		const auto& max_1 = c1.max();
		const auto& min_1 = c1.min();
		const auto& max_2 = c2.max();
		const auto& min_2 = c2.max();

		// The extents overlap if the min/max coords overlap on both axes. 
		return (max_1.x >= min_2.x) && (min_1.x <= max_2.x)
			&& (max_1.y >= min_2.y) && (min_1.y <= max_2.y);
	}
	
	//-------------------------------------------------------------------------------------

	template<>
	bool overlaps(const Extent& c1, const Circle& c2)
	{
		// By getting a vector from the extent's centre to the circle centre and clamping
		// its length by the half extent. This effectively clamps the vector so that 
		// it always ends within the AABB, hence its the closest point to the circle
		// centre that is within the AABB. If this point is also within the circle
		// then they overlap. 

		const glm::vec2 half_extent = c1.size() * 0.5f;
		const glm::vec2 centre_to_centre = c2.centre() - c1.centre();
		const glm::vec2 closest_point = glm::clamp(centre_to_centre, -half_extent, half_extent);

		return glm::distance2(c2.centre(), closest_point) <= c2.radius() * c2.radius();
	}
	
	//-------------------------------------------------------------------------------------

	template<>
	bool overlaps(const Extent& c1, const Polygon& c2)
	{
		// We need to check for overlap using the separating axis theorem. Since the
	    // extent has two parallel sides, we just need to add two of its normals 
	    // to the polygon's normals for the SAT. 

		const auto& mesh_1 = c1.mesh();
		const auto& mesh_2 = c2.mesh();

		const auto& vertices_1 = mesh_1.vertices();
		const auto& vertices_2 = mesh_2.vertices();

		// Only use two of the normals since the sides are parallel
		constexpr glm::vec2 vert_axes{0.0f, 1.0f};
		constexpr glm::vec2 horz_axes{1.0f, 0.0f};

		const auto& normals_2 = mesh_2.normals();
		const std::array<glm::vec2, 2> normals_1 {
			horz_axes, horz_axes
		};

		return test_separating_axis_theorem(
			std::span(normals_1.begin(), normals_1.end()),
			std::span(normals_2.begin(), normals_2.end()),
			std::span(vertices_1.begin(), vertices_1.end()),
			std::span(vertices_2.begin(), vertices_2.end())
		);
	}
	
	//-------------------------------------------------------------------------------------

	template<>
	bool overlaps(const Extent& c1, const Rectangle& c2)
	{
		// We need to check for overlap using the separating axis theorem. 
		// However, since two sides of the rectangle and extent are parallel, 
		// we can do it with half the axes as an optimisation. 

		const auto& mesh_1 = c1.mesh();
		const auto& mesh_2 = c2.mesh();

		const auto& vertices_1 = mesh_1.vertices();
		const auto& vertices_2 = mesh_2.vertices();

		const auto& normals_2 = mesh_2.normals();
		constexpr glm::vec2 vert_axes{0.0f, 1.0f};
		constexpr glm::vec2 horz_axes{1.0f, 0.0f};

		const std::array<glm::vec2, 4> axes {
			horz_axes, vert_axes, normals_2[0], normals_2[3]
		};

		return test_separating_axis_theorem(
			std::span(axes),
			std::span(vertices_1),
			std::span(vertices_2)
		);
	}
	
	//-------------------------------------------------------------------------------------

	template<>
	bool contains(const Extent& c, const glm::vec2& p)
	{
		const auto& max = c.max();
		const auto& min = c.min();

		return p.x >= min.x
			&& p.x <= max.x
			&& p.y <= max.y
			&& p.y >= min.y;
	}

	//-------------------------------------------------------------------------------------

	template<>
	bool contains<Extent, Extent>(const Extent& c1, const Extent& c2)
	{
		const auto& outer_min = c1.min();
		const auto& outer_max = c1.max();
		const auto& inner_min = c2.min();
		const auto& inner_max = c2.max();
	

		return (inner_min.x >= outer_min.x && inner_min.y >= outer_min.y)
			&& (inner_max.x <= outer_max.x && outer_max.y <= outer_max.y);
	}

	//-------------------------------------------------------------------------------------

}