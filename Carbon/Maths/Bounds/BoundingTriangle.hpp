#pragma once

#include <array>
#include <tuple>
#include <glm/glm.hpp>

#include "BoundingBox.hpp"
#include "BoundingCircle.hpp"

#include "../Transform.hpp"

namespace cbn
{

	class BoundingBox;
	class BoundingCircle;

	struct TriangleMesh
	{
		// Vertices
		/*
			Vertices are counter clockwise from first vertex
				
					 1
				    / \
				   /   \
				  /     \
				 2-------3
		*/
		union
		{
			struct
			{
				glm::vec2 vertex_1;
				glm::vec2 vertex_2;
				glm::vec2 vertex_3;
			};
			glm::vec2 vertices[3];
		};

		// Normals
		/*
			Normals are counter clockwise from first vertex
			         *
				    / \
				 1 /   \ 3
				  /     \
				 *-------*
				     2
		*/
		union
		{
			struct
			{
				glm::vec2 normal_1;
				glm::vec2 normal_2;
				glm::vec2 normal_3;
			};
			glm::vec2 normals[3];
		};
	};

	class BoundingTriangle
	{
	private:

		glm::vec2 m_LocalVertex1, m_LocalVertex2, m_LocalVertex3;
		std::tuple<glm::vec2, glm::vec2> m_MinMaxCoords;
		glm::vec2 m_LocalCentre;
		Transform m_Transform;
		TriangleMesh m_Mesh;
		glm::vec2 m_Centre;

		void update_min_max_coords();

	public:

		BoundingTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& local_origin = {0,0});

		BoundingTriangle(const Transform& transform, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& local_origin = {0,0});

		bool overlaps(const BoundingBox& box) const;

		bool overlaps(const BoundingCircle& circle) const;

		bool overlaps(const BoundingTriangle& triangle) const;

		bool encloses(const BoundingBox& box) const;

		bool encloses(const BoundingCircle& circle) const;

		bool encloses(const BoundingTriangle& triangle) const;

		bool contains_point(const glm::vec2& point) const;

		bool is_intersected_by_line(const glm::vec2& p1, const glm::vec2& p2) const;

		bool is_intersected_by_line_segment(const glm::vec2& p1, const glm::vec2& p2) const;

		bool is_intersected_by_ray(const glm::vec2& origin, const glm::vec2& towards) const;

		void reshape(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& local_origin = {0,0});

		void transform_to(const Transform& transform);

		void transform_by(const Transform& transform);

		const glm::vec2& centre() const;

		const glm::vec2& origin() const;

		const TriangleMesh& mesh() const;

		BoundingBox wrap_axis_alligned() const;

		const std::tuple<glm::vec2, glm::vec2>& min_max_coords() const;
	};

}