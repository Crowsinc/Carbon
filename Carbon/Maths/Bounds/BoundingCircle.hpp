#pragma once

#include <glm/glm.hpp>


#include "../Transform.hpp"
#include "BoundingArea.hpp"
#include "BoundingBox.hpp"

namespace cbn
{
	//TODO: if we add origin, add caching too
	class BoundingCircle : public virtual BoundingArea, public Transformable<Translatable2D, Rotatable2D, Scalable1D>
	{
	private:

		float m_Radius;

	public:

		BoundingCircle(const float radius);

		BoundingCircle(const Transform& transform, const float radius);

		bool overlaps(const BoundingArea& area) const override;

		bool overlaps(const BoundingBox& box) const override;

		bool overlaps(const BoundingCircle& circle) const override;

		bool encloses(const BoundingBox& box) const override;

		bool encloses(const BoundingCircle& circle) const override;

		bool contains_point(const glm::vec2& point) const override;

		bool is_intersected_by_line(const glm::vec2& p1, const glm::vec2& p2) const override;

		bool is_intersected_by_ray(const glm::vec2& origin, const glm::vec2& towards) const override;

		BoundingCircle wrap_as_bounding_circle() const override;

		BoundingBox wrap_as_axis_alligned() const override;

		BoundingBox wrap_as_bounding_box() const override;

		float scaled_diameter() const;

		float scaled_radius() const;

		glm::vec2 centre() const;

		float diameter() const;

		float radius() const;
	};

}