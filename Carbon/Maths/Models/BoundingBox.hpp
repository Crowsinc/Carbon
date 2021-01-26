#pragma once

#include "Mesh.hpp"
#include "BoundingCircle.hpp"
#include "BoundingTriangle.hpp"
#include "../Physics/Collider.hpp"

#include "../Transforms/Transformable.hpp"

namespace cbn
{

	class BoundingBox : public Collider, public Transformable<Translatable2D, Rotatable2D>
	{
	private:

		glm::vec2 m_Size;
		Extent m_LocalExtent;
		glm::vec2 m_LocalOriginOffset;

		// Caching
		mutable bool m_ExtentOutdated, m_AllignmentOutdated;
		mutable bool m_MeshOutdated, m_CentreOutdated;
		mutable bool m_AxisAlligned;
		mutable Extent m_Extent;
		mutable QuadMesh m_Mesh;
		mutable Point m_Centre;
		
		Point transform_to_local(const Point& point) const;

		void on_transform() override;

		void generate_mesh() const;

		void update_extent() const;

	public:

		BoundingBox(const Extent& extent);

		BoundingBox(const BoundingBox& other) = default;

		BoundingBox(const glm::vec2& size, const glm::vec2& origin_offset = {0,0}, bool local_coords = false);

		BoundingBox(const Transform& transform, const glm::vec2& size, const glm::vec2& origin_offset = {0,0}, bool local_coords = false);

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