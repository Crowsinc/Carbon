#pragma once

#include "Mesh.hpp"
#include "BoundingBox.hpp"
#include "BoundingCircle.hpp"
#include "../Physics/Collider.hpp"

#include "../Transforms/Transformable.hpp"

namespace cbn
{

	class BoundingBox;
	class BoundingCircle;

	class BoundingTriangle : public Collider, public Transformable<Translatable2D, Rotatable2D>
	{
	private:

		TriangleMesh::Vertices m_LocalVertices;
		glm::vec2 m_LocalOriginOffset;

		mutable bool m_DirectionOutdated, m_MeshOutdated;
		mutable bool m_ExtentOutdated, m_CentreOutdated;
		mutable glm::vec2 m_Direction;
		mutable TriangleMesh m_Mesh;
		mutable Extent m_Extent;
		mutable Point m_Centre;

		Extent create_extent(const TriangleMesh::Vertices& vertices) const;

		void on_transform() override;

	public:

		BoundingTriangle(const BoundingTriangle& other) = default;

		BoundingTriangle(const TriangleMesh::Vertices& vertices, const glm::vec2& origin_offset = {0,0}, const bool local_coords = false);

		BoundingTriangle(const Transform& transform, const TriangleMesh::Vertices& vertices, const glm::vec2& origin_offset = {0,0}, const bool local_coords = false);

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

		void reshape(const TriangleMesh::Vertices& vertices);

		void specify_origin(const glm::vec2& origin_offset, const bool local_coords = false) override;
		 
		const glm::vec2& direction() const override;

		const Point& centre() const override;
		
		const Point& origin() const override;

		const TriangleMesh& mesh() const;

		BoundingBox wrap_axis_alligned() const;

		BoundingBox wrap_object_alligned() const;

	};

}