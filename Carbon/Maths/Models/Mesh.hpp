#pragma once

#include <array>
#include <algorithm>
#include <glm/glm.hpp>

#include "Line.hpp"
#include "../Transforms/Transform.hpp"

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

	template<uint64_t V>
	class StaticMesh
	{
		static_assert(V >= 3, "A mesh cannot have less than three vertices");
	private:

		std::array<glm::vec2, V> m_Vertices, m_Normals;
		std::array<Segment, V> m_Edges;
	
		void generate_static_mesh(const std::array<glm::vec2, V>& vertices);

	public:

		StaticMesh();

		StaticMesh(const StaticMesh& mesh);

		StaticMesh(const std::array<glm::vec2, V>& vertices);
		
		StaticMesh(const Transform& transform, const std::array<glm::vec2, V>& vertices);

		const std::array<glm::vec2, V>& vertices() const;

		const std::array<glm::vec2, V>& normals() const;
		
		const std::array<Segment, V>& edges() const;

	};

	class DynamicMesh
	{
	private:

		std::vector<glm::vec2> m_Vertices, m_Normals;
		std::vector<Segment> m_Edges;

		void generate_mesh(const std::vector<glm::vec2>& vertices);

	public:

		DynamicMesh();

		DynamicMesh(const std::vector<glm::vec2>& vertices);

		DynamicMesh(const Transform& transform, const std::vector<glm::vec2>& vertices);

		const std::vector<glm::vec2>& vertices() const;

		const std::vector<glm::vec2>& normals() const;
		
		const std::vector<Segment>& edges() const;

		const uint64_t sides() const;
	
	};
	
	//-------------------------------------------------------------------------------------

	template<uint64_t V>
	inline StaticMesh<V>::StaticMesh() {}
	
	//-------------------------------------------------------------------------------------

	template<uint64_t V>
	inline StaticMesh<V>::StaticMesh(const StaticMesh<V>& mesh)
		: m_Vertices(mesh.m_Vertices),
		  m_Normals(mesh.m_Normals),
		  m_Edges(mesh.m_Edges)
	{}

	//-------------------------------------------------------------------------------------

	template<uint64_t V>
	inline StaticMesh<V>::StaticMesh(const std::array<glm::vec2, V>& vertices)
	{
		generate_static_mesh(vertices);
	}
	
	//-------------------------------------------------------------------------------------

	template<uint64_t V>
	inline StaticMesh<V>::StaticMesh(const Transform& transform, const std::array<glm::vec2, V>& vertices)
	{
		std::array<glm::vec2, V> transformed_vertices;
		std::transform(vertices.begin(), vertices.end(), transformed_vertices.begin(), [&](const auto& v)
		{
			return transform.apply(v);
		});
		
		generate_static_mesh(transformed_vertices);
	}
	
	//-------------------------------------------------------------------------------------

	template<uint64_t V>
	inline void StaticMesh<V>::generate_static_mesh(const std::array<glm::vec2, V>& vertices)
	{
		m_Vertices = vertices;

		// Use the vertices to find the normals and edges
		glm::vec2 last_vertex = vertices[V - 1];
		for (auto i = 0; i < V; i++)
		{
			const glm::vec2& vertex = vertices[i];

			m_Edges[i] = Segment(last_vertex, vertex);
			m_Normals[i] = m_Edges[i].normal();

			last_vertex = vertex;
		}
	}

	//-------------------------------------------------------------------------------------

	template<uint64_t V>
	inline const std::array<glm::vec2, V>& StaticMesh<V>::vertices() const
	{
		return m_Vertices;
	}
	
	//-------------------------------------------------------------------------------------

	template<uint64_t V>
	inline const std::array<glm::vec2, V>& StaticMesh<V>::normals() const
	{
		return m_Normals;
	}
	
	//-------------------------------------------------------------------------------------

	template<uint64_t V>
	inline const std::array<Segment, V>& StaticMesh<V>::edges() const
	{
		return m_Edges;
	}
	
	//-------------------------------------------------------------------------------------

}
