#pragma once

#include "Mesh.hpp"
#include "BoundingCircle.hpp"
#include "BoundingTriangle.hpp"
#include "../Physics/Collider.hpp"

namespace cbn
{

	class BoundingBox : public Collider
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

		void generate_mesh() const;

		void update_extent() const;

	public:

		BoundingBox(const glm::vec2& size);

		BoundingBox(const Extent& extent);

		BoundingBox(const Transform& transform, const glm::vec2& size);

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

		void resize(const glm::vec2& size);

		void specify_origin(const glm::vec2& origin_offset, bool local_coords = false) override;

		const glm::vec2& direction() const override;

		const Point& centre() const override;

		const Point& origin() const override;

		const glm::vec2& size() const;

		const QuadMesh& mesh() const;

		bool is_axis_alligned() const;

		BoundingBox wrap_axis_alligned() const;

	};

}