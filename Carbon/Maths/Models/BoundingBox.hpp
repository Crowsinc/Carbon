#pragma once

#include "Meshes.hpp"
#include "BoundingCircle.hpp"
#include "BoundingTriangle.hpp"
#include "../Physics/Collider.hpp"

#include "../Transform.hpp"

namespace cbn
{

	class BoundingBox : public Collider, public Transformable<Translatable2D, Rotatable2D>
	{
	private:

		Extent m_LocalExtent;
		glm::vec2 m_LocalOriginOffset;

		glm::vec2 m_Size;
		glm::vec2 m_Direction;

		//TODO: proper caching
		mutable Extent m_Extent;
		mutable bool m_AxisAlligned;
		mutable QuadMesh m_Mesh;
		mutable Point m_Centre;
		
		Point transform_to_local(const Point& point) const;

		void update_extent() const;

	public:

		BoundingBox(const glm::vec2& size);

		BoundingBox(const Extent& extent);

		BoundingBox(const Transform& transform, const glm::vec2& size);

		bool overlaps(const Collider& collider) const override;

		bool overlaps(const BoundingBox& box) const override;

		bool overlaps(const BoundingCircle& circle) const override;

		bool overlaps(const BoundingTriangle& triangle) const override;

		bool encloses(const Collider& collider) const override;

		bool encloses(const BoundingBox& box) const override;

		bool encloses(const BoundingCircle& circle) const override;

		bool encloses(const BoundingTriangle& triangle) const override;

		bool contains(const Point& point) const override;

		bool intersected_by(const Line& line) const override;

		bool intersected_by(const Segment& segment) const override;

		bool intersected_by(const Ray& ray) const override;

		const Extent& extent() const override;

		void resize(const glm::vec2& size);

		void specify_origin(const glm::vec2& origin_offset, bool local_coords = false);

		const glm::vec2& direction() const;

		const Point& centre() const;

		const Point& origin() const;

		const glm::vec2& size() const;

		const QuadMesh& mesh() const;

		bool is_axis_alligned() const;

		BoundingBox wrap_axis_alligned() const;

	};

}