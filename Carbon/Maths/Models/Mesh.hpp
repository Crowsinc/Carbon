#pragma once

#include <array>
#include <glm/glm.hpp>

#include "Common.hpp"
#include "../Transform.hpp"

namespace cbn
{

	/*
							CONVENTIONS!	 

		Vertices are specified in a counter clockwise order
			
			0---------3				 0
			|		  |  			/ \
			|         |		       /   \
			|         |		      /     \
			1---------2		     1-------2
			 	
		Edges & Normals start between the first and last vertex, and follow the vertex order
								
				 0				
			*---------*				 *
			|         |				/ \
		  1 |         | 4		 1 /   \ 0
			|         |			  /     \
			*---------*			 *-------*
				 2					 2
	*/

	template<unsigned V>
	struct PolygonMesh
	{
		static_assert(V >= 3, "Cannot have less then three vertices");
		
		using Normals = std::array<glm::vec2, V>;
		using Vertices = std::array<Point, V>;
		using Edges = std::array<Segment, V>;

		Vertices vertices;
		Normals normals;
		Edges edges;

		static PolygonMesh Create(const Transform& transform, const Vertices& vertices);
		
		static PolygonMesh Create(const Vertices& vertices);

	};

	// Common Polygons
	using TriangleMesh = PolygonMesh<3>;
	using QuadMesh = PolygonMesh<4>;

	//-------------------------------------------------------------------------------------

	template<unsigned V>
	inline PolygonMesh<V> PolygonMesh<V>::Create(const Transform& transform, const Vertices& vertices)
	{
		// Apply the transform to the vertices, then use the transformless create function
		std::array<glm::vec2, V> transformed_vertices;
		std::transform(vertices.begin(), vertices.end(), transformed_vertices.begin(), [&](const Point& vertex)
		{
			return transform.apply(vertex);
		});

		return PolygonMesh<V>::Create(transformed_vertices);
	}
	
	//-------------------------------------------------------------------------------------

	template<unsigned V>
	inline PolygonMesh<V> PolygonMesh<V>::Create(const Vertices& vertices)
	{
		PolygonMesh<V> mesh;
		mesh.vertices = vertices;

		glm::vec2 last_vertex = vertices[V - 1];
		for(auto i = 0; i < V; i++)
		{
			const auto& vertex = vertices[i];

			mesh.edges[i].p1 = last_vertex;
			mesh.edges[i].p2 = vertex;

			// The normal is the counter clockwise 90 degree rotation of the 
			// edge direction, going from the current vertex to the last one
			const auto edge_dir = glm::normalize(last_vertex - vertex);
			mesh.normals[i].x = -edge_dir.y;
			mesh.normals[i].y =  edge_dir.x;

			last_vertex = vertex;
		}

		return mesh;
	}
	
	//-------------------------------------------------------------------------------------

}