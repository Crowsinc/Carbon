#pragma once

#include "Mesh.hpp"
#include "BoundingBox.hpp"
#include "BoundingCircle.hpp"
#include "../Physics/Collider.hpp"


namespace cbn
{

	class BoundingBox;
	class BoundingCircle;

	class BoundingTriangle : public Collider
	{
	private:

		TriangleMesh::Vertices m_LocalVertices;
		glm::vec2 m_LocalOriginOffset;

		mutable glm::vec2 m_Direction;
		mutable Point m_Centre;
		mutable TriangleMesh m_Mesh;
		mutable Extent m_Extent;

		Extent create_extent(const TriangleMesh::Vertices& vertices) const;

	public:

		BoundingTriangle(const TriangleMesh::Vertices& vertices);

		BoundingTriangle(const Transform& transform, const TriangleMesh::Vertices& vertices);

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