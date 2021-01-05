#pragma once

#include <glm/glm.hpp>


#include "../Transform.hpp"

#include "BoundingBox.hpp"
#include "BoundingTriangle.hpp"

namespace cbn
{

	class BoundingBox;
	class BoundingTriangle;

	class BoundingCircle
	{
	private:

		glm::vec2 m_LocalCentre;

		float m_Radius;
		glm::vec2 m_Centre;
		Transform m_Transform;
		std::tuple<glm::vec2, glm::vec2> m_MinMaxCoords;
		
		void update_min_max_coords();

	public:


		BoundingCircle(const float radius, const glm::vec2& local_origin = {0,0});

		BoundingCircle(const Transform& transform, const float radius, const glm::vec2& local_origin = {0,0});

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

		void reshape(const float radius, const glm::vec2& local_origin = {0,0});

		void transform_to(const Transform& transform);

		void transform_by(const Transform& transform);

		const glm::vec2& centre() const;

		const glm::vec2& origin() const;

		const float radius() const;

		BoundingBox wrap_axis_alligned() const;

		const std::tuple<glm::vec2, glm::vec2>& min_max_coords() const;

	};

}