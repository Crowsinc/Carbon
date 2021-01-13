#pragma once

#include <array>
#include <glm/glm.hpp>

#include "Common.hpp"
#include "../Transform.hpp"

namespace cbn
{

	template<unsigned V>
	struct PolygonMesh
	{
		static_assert(V >= 3, "Cannot have less then three vertices");

		constexpr static unsigned Sides = V;

		static PolygonMesh Create(const Transform& transform, const std::array<Point, V>& vertices);
		
		static PolygonMesh Create(const std::array<Point, V>& vertices);

		std::array<Point, V> vertices;
		std::array<Segment, V> edges;
		std::array<glm::vec2, V> normals;
	};

	// 3 vertex polygons
	struct TriangleMesh : PolygonMesh<3>
	{

		TriangleMesh();

		TriangleMesh(const TriangleMesh& mesh);

		TriangleMesh(const PolygonMesh<3>& polygon);

		void operator=(const TriangleMesh& mesh);

		void operator=(const PolygonMesh<3>& polygon);

		/*
			Vertices are counter clockwise from first vertex

					 1
					/ \
				   /   \
				  /     \
				 2-------3
		*/
		Point& vertex_1 = vertices[0];
		Point& vertex_2 = vertices[1];
		Point& vertex_3 = vertices[2];

		/*
			Edges are counter clockwise from the first-last vertex
					 *
					/ \
				 2 /   \ 1
				  /     \
				 *-------*
					 3
		*/
		Segment& edge_1 = edges[0];
		Segment& edge_2 = edges[1];
		Segment& edge_3 = edges[2];


		/*
			Normals are counter clockwise from the first edge
					 *
					/ \
				 2 /   \ 1
				  /     \
				 *-------*
					 3
		*/
		glm::vec2& normal_1 = normals[0];
		glm::vec2& normal_2 = normals[1];
		glm::vec2& normal_3 = normals[2];

	};

	// 4 vertex polygons
	struct QuadMesh : PolygonMesh<4>
	{
		static QuadMesh Create(const Transform& transform, const glm::vec2& size, const glm::vec2& local_origin = {0,0});
		
		static QuadMesh Create(const Extent& extent);

		QuadMesh();

		QuadMesh(const QuadMesh& mesh);

		QuadMesh(const PolygonMesh<4>& polygon);

		void operator=(const QuadMesh& mesh);

		void operator=(const PolygonMesh<4>& polygon);

		/*
			Vertices are counter clockwise from top left with no rotation
				1---------4
				|         |
				|         |
				2---------3
		*/
		Point& vertex_1 = vertices[0];
		Point& vertex_2 = vertices[1];
		Point& vertex_3 = vertices[2];
		Point& vertex_4 = vertices[3];

		/*
			Edges are counter clockwise from the first-last vertex
					 1
				*---------*
				|         |
			  2 |         | 4
				|         |
				*---------*
					 3
		*/
		Segment& edge_1 = edges[0];
		Segment& edge_2 = edges[1];
		Segment& edge_3 = edges[2];
		Segment& edge_4 = edges[3];


		/*
			Normals are counter clockwise from the first edge
					 1
				*---------*
				|         |
			  2 |         | 4
				|         |
				*---------*
					 3
		*/
		glm::vec2& normal_1 = normals[0];
		glm::vec2& normal_2 = normals[1];
		glm::vec2& normal_3 = normals[2];
		glm::vec2& normal_4 = normals[3];
	};

	//-------------------------------------------------------------------------------------

	template<unsigned V>
	inline PolygonMesh<V> PolygonMesh<V>::Create(const Transform& transform, const std::array<glm::vec2, V>& vertices)
	{
		// Apply the transform to the vertices, then use the transformless create function
		std::array<glm::vec2, V> transformed_vertices;
		std::transform(vertices.begin(), vertices.end(), transformed_vertices.begin(), [&](const glm::vec2& vertex)
		{
			return transform.apply(vertex);
		});

		return PolygonMesh<V>::Create(transformed_vertices);
	}
	
	//-------------------------------------------------------------------------------------

	template<unsigned V>
	inline PolygonMesh<V> PolygonMesh<V>::Create(const std::array<glm::vec2, V>& vertices)
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