#pragma once

#include "Mesh.hpp"
#include "BoundingBox.hpp"
#include "BoundingTriangle.hpp"
#include "../Physics/Collider.hpp"

#include "../Transform.hpp"

namespace cbn
{

	class BoundingBox;
	class BoundingTriangle;

	class BoundingCircle : public Collider, public Transformable<Translatable2D, Rotatable2D>
	{
	private:

		float m_Radius;
		glm::vec2 m_LocalCentreOffset;

		mutable Extent m_Extent;
		mutable Point m_Centre;
		mutable glm::vec2 m_Direction;

		void update_extent() const;

	public:

		BoundingCircle(const float radius);

		BoundingCircle(const Transform& transform, const float radius);

		bool overlaps(const Collider& collider) const override;

		bool overlaps(const BoundingBox& box) const override;

		bool overlaps(const BoundingCircle& circle) const override;

		bool overlaps(const BoundingTriangle& triangle) const override;

		bool enclosed_by(const Collider& collider) const override;

		bool encloses(const BoundingBox& box) const override;

		bool encloses(const BoundingCircle& circle) const override;

		bool encloses(const BoundingTriangle& triangle) const override;

		bool contains(const Point& point) const override;

		bool intersected_by(const Line& line) const override;

		bool intersected_by(const Segment& segment) const override;

		bool intersected_by(const Ray& ray) const override;

		const Extent& extent() const override;

		void resize(const float radius);

		void specify_origin(const glm::vec2& origin_offset, const bool local_coords = false);

		const glm::vec2& direction() const;

		const Point& centre() const;

		const Point& origin() const;

		const float radius() const;

		BoundingBox wrap_axis_alligned() const;

	};

}