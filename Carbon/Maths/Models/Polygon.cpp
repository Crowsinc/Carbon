#include "Polygon.hpp"

#include <algorithm>
#include <numeric>
#include <span>

#include "../../Algorithms/SeparatingAxisTheorem.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------
	
	Polygon::Polygon()
    {}

	//-------------------------------------------------------------------------------------

	Polygon::Polygon(const DynamicMesh& mesh)
    {
		reshape(mesh);
	}

	//-------------------------------------------------------------------------------------

	Polygon::Polygon(const Transform& transform, const DynamicMesh& mesh)
	{
		reshape(mesh);
		this->transform_to(transform);
	}
	
	//-------------------------------------------------------------------------------------
	
	void Polygon::reshape(const DynamicMesh& mesh)
    {
		m_Mesh = mesh;
	}
	
	//-------------------------------------------------------------------------------------
	
	glm::vec2 Polygon::centroid() const
	{
		const auto& vertices = m_Mesh.vertices();
		double x_sum = 0, y_sum = 0;

		for(const auto& vertex : vertices)
		{
			x_sum += vertex.x;
			y_sum += vertex.y;
		}

		return {x_sum / vertices.size(), y_sum / vertices.size()};
    }
	
	//-------------------------------------------------------------------------------------
	
	uint64_t Polygon::sides() const
    {
        return m_Mesh.sides();
    }
	
	//-------------------------------------------------------------------------------------
	
	const Extent Polygon::extent() const
    {
		const auto& vertices = m_Mesh.vertices();

		const auto [min_x, max_x] = std::minmax_element(vertices.begin(), vertices.end(), [](const glm::vec2& a, const glm::vec2& b)
		{
			return a.x < b.x;
		});
		const auto [min_y, max_y] = std::minmax_element(vertices.begin(), vertices.end(), [](const glm::vec2& a, const glm::vec2& b)
		{
			return a.y < b.y;
		});

		return Extent{{min_x->x, min_y->y}, {max_x->x, max_y->y}};
    }
	
	//-------------------------------------------------------------------------------------

    const DynamicMesh& Polygon::mesh() const
    {
		return m_Mesh;
    }
	
	//-------------------------------------------------------------------------------------

	template<>
	bool overlaps(const Polygon& c1, const Polygon& c2)
	{
		// Simply do SAT
		const auto& mesh_1 = c1.mesh();
		const auto& mesh_2 = c2.mesh();

		const auto& normals_1 = mesh_1.normals();
		const auto& normals_2 = mesh_2.normals();

		const auto& vertices_1 = mesh_1.vertices();
		const auto& vertices_2 = mesh_2.vertices();

		return test_separating_axis_theorem(
			std::span(normals_1.begin(), normals_1.end()),
			std::span(normals_2.begin(), normals_2.end()),
			std::span(vertices_1.begin(), vertices_1.end()), 
			std::span(vertices_2.begin(), vertices_2.end())
		);
	}
	
	//-------------------------------------------------------------------------------------

	//template<>
	//bool overlaps(const Polygon& c1, const Circle& c2)
	//{
	//	const auto& mesh = c1.mesh();
	//	const auto& edges = mesh.edges();
	//	const auto& vertices = mesh.vertices();

	//	// The circle intersects the polygon if its centre is inside the polygon's centre,
	//	// any of the polygon's vertices are inside the circle, or the circle overlaps with
	//	// any of the polygon's edges. 

	//	// Test trivial cases where circle centre is within polygon or polygon vertex is within circle
	//	return c1.contains(c2.centre())
	//		|| std::any_of(vertices.begin(), vertices.end(), [&](const auto& v) { return c2.contains(v); })
	//		|| std::any_of(edges.begin(), edges.end(), [&](const auto& e) { return c2.collides(e); });
	//}
	
	//-------------------------------------------------------------------------------------

	template<>
	bool overlaps(const Polygon& c1, const Extent& c2)
	{
		//Re-use the overlap from the extent's side
		return overlaps(c2, c1);
	}
	
	//-------------------------------------------------------------------------------------

	template<>
	bool overlaps(const Polygon& c1, const Rectangle& c2)
	{
		//Re-use the overlap from the rectangles's side
		return overlaps(c2, c1);
	}
	
	//-------------------------------------------------------------------------------------

}