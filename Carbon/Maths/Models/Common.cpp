#include "Common.hpp"


namespace cbn
{
	//-------------------------------------------------------------------------------------

	bool Extent::overlaps(const Extent& other) const
	{
		// The extents overlap if the min/max coords overlap on both axes. 
		return (max.x >= other.min.x) && (min.x <= other.max.x)
			&& (max.y >= other.min.y) && (min.y <= other.max.y);
	}

	//-------------------------------------------------------------------------------------
	
	bool Extent::contains(const Point& point) const
	{
		return point.x >= min.x
			&& point.x <= max.x
			&& point.y <= max.y
			&& point.y >= min.y;
	}
	
	//-------------------------------------------------------------------------------------
	
	//	QuadMesh::Vertices Extent::vertices() const
	std::array<Point, 4> Extent::vertices() const
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

		//QuadMesh::Vertices vertices{};
		std::array<Point, 4> vertices{};

		// Vertex 0
		vertices[0].x = min.x;
		vertices[0].y = max.y;

		// Vertex 1
		vertices[1].x = min.x;
		vertices[1].y = min.y;

		// Vertex 2
		vertices[2].x = max.x;
		vertices[2].y = min.y;

		// Vertex 3
		vertices[3].x = max.x;
		vertices[3].y = max.y;

		return vertices;
	}

	//-------------------------------------------------------------------------------------

}