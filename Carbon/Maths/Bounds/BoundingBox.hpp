#pragma once

#include <array>
#include <tuple>
#include <glm/glm.hpp>

#include "BoundingCircle.hpp"
#include "BoundingTriangle.hpp"

#include "../Transform.hpp"

namespace cbn
{

	class BoundingCirlce;
	class BoundingTriangle;

	struct BoxMesh
	{
		// Vertices
		/*
			Vertices are counter clockwise from top left with no rotation
				1---------4
				|         |
				|         |
				2---------3
		*/
		union
		{
			struct
			{
				glm::vec2 vertex_1;
				glm::vec2 vertex_2;
				glm::vec2 vertex_3;
				glm::vec2 vertex_4;
			};
			glm::vec2 vertices[4];
		};

		// Normals
		/*
			Normals are counter clockwise from leftmost edge with no rotation
					
					 4
				*---------*
				|         |
			  1 |         | 3
				|         |
				*---------*
				     2
		*/
		union
		{
			struct
			{
				glm::vec2 normal_1;
				glm::vec2 normal_2;
				glm::vec2 normal_3;
				glm::vec2 normal_4;
			};
			glm::vec2 normals[4];
		};
	};

	class BoundingBox
	{
	private:

		glm::vec2 m_LocalVertex1, m_LocalVertex2, m_LocalVertex3, m_LocalVertex4;
		std::tuple<glm::vec2, glm::vec2> m_MinMaxCoords;
		glm::vec2 m_LocalOriginOffset;
		glm::vec2 m_LocalCentre;
		Transform m_Transform;
		bool m_AxisAlligned;
		glm::vec2 m_Centre;
		glm::vec2 m_Size;
		BoxMesh m_Mesh;

		void update_min_max_coords();

		glm::vec2 transform_to_local_space(const glm::vec2& point) const;

	public:

		BoundingBox(const glm::vec2& size, const glm::vec2& local_origin = {0,0});

		BoundingBox(const Transform& transform, const glm::vec2& size, const glm::vec2& local_origin = {0,0});

		bool overlaps(const BoundingBox& box, const float axis_snap_threshold = 0.1f) const;

		bool overlaps(const BoundingCircle& circle) const;

		bool overlaps(const BoundingTriangle& triangle) const;

		bool encloses(const BoundingBox& box) const;

		bool encloses(const BoundingCircle& circle) const;

		bool encloses(const BoundingTriangle& triangle) const;

		bool contains_point(const glm::vec2& point) const;

		bool is_intersected_by_line(const glm::vec2& p1, const glm::vec2& p2) const;

		bool is_intersected_by_line_segment(const glm::vec2& p1, const glm::vec2& p2) const;

		bool is_intersected_by_ray(const glm::vec2& origin, const glm::vec2& towards) const;

		void reshape(const glm::vec2& size, const glm::vec2& local_origin = {0,0});

		void transform_to(const Transform& transform);

		void transform_by(const Transform& transform);

		const glm::vec2& centre() const;

		const glm::vec2& origin() const;

		const glm::vec2& size() const;

		const BoxMesh& mesh() const;

		bool is_axis_alligned() const;

		BoundingBox wrap_axis_alligned() const;

		const std::tuple<glm::vec2, glm::vec2>& min_max_coords() const;
	};

}