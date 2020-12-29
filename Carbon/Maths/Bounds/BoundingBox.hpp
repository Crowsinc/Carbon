#pragma once

#include <array>
#include <tuple>
#include <glm/glm.hpp>

#include "BoundingArea.hpp"
#include "BoundingCircle.hpp"

#include "../Transform.hpp"

namespace cbn
{

	struct BoxMesh
	{
		glm::vec2 vertex_1;
		glm::vec2 vertex_2;
		glm::vec2 vertex_3;
		glm::vec2 vertex_4;

		std::array<glm::vec2, 4> as_array() const;
	};

	class BoundingBox : public Transformable<Translatable2D, Scalable2D, Rotatable2D> 
	{
	private:

		glm::vec2 m_Size;
		glm::vec2 m_LocalOrigin;
		glm::vec2 m_LocalCentre;
		BoxMesh m_LocalVertices;

		mutable bool m_CacheOutdated;
		mutable BoxMesh m_CachedVertices;
		mutable glm::vec2 m_CachedCentre;

		void generate_local_mesh();

	public:
		
		BoundingBox(const glm::vec2& size, const glm::vec2& local_origin = {0,0});
		
		BoundingBox(const Transform& transform, const glm::vec2& size, const glm::vec2& local_origin = {0,0});

		bool overlaps(const BoundingArea& area) const;

		bool overlaps(const BoundingBox& box) const;

		bool overlaps(const BoundingCircle& circle) const;

		bool encloses(const BoundingBox& box) const;

		bool encloses(const BoundingCircle& circle) const;

		bool contains_point(const glm::vec2& point) const;

		bool is_intersected_by_line(const glm::vec2& p1, const glm::vec2& p2) const;

		bool is_intersected_by_ray(const glm::vec2& origin, const glm::vec2& towards) const;

		std::tuple<float, float, float, float> find_min_max_coords() const;

		void resize(const glm::vec2& size, const glm::vec2& local_origin = {0,0});

		void set_local_origin(const glm::vec2& local_origin);
		
		BoundingCircle wrap_as_bounding_circle() const; 

		BoundingBox wrap_as_bounding_box() const;

		BoundingBox wrap_as_axis_alligned() const;

		bool is_axis_alligned(const float threshold = 0.1f) const;

		void allign_to_axis();

		const BoxMesh& local_vertices() const;

		glm::vec2 local_centre() const;

		glm::vec2 local_origin() const;
		
		void TEMP_UPDATE_CACHE() const; /// ---------------------------------------------- remove
		
		const BoxMesh& vertices() const;

		glm::vec2 centre() const;

		glm::vec2 size() const;

		glm::vec2 scaled_size() const;

	};

}

