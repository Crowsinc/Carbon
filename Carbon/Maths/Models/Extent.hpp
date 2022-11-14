#pragma once

#include "../Transforms/Transformable.hpp"
#include "../Physics/Collider.hpp"
#include "Mesh.hpp"

#include "Circle.hpp"
#include "Polygon.hpp"
#include "Rectangle.hpp"

namespace cbn
{

	class Extent : public Collider, public Transformable<Translatable2D>
	{
	private:

		glm::vec2 m_HalfSize;

	public:

		Extent();

		Extent(const glm::vec2 size);
		
		Extent(const glm::vec2 min, const glm::vec2 max);

		void resize(const glm::vec2 size);

		void reshape(const glm::vec2 min, const glm::vec2 max);

		glm::vec2 min() const;
		
		glm::vec2 max() const;

		glm::vec2 size() const;

		const glm::vec2& centre() const;

		StaticMesh<4> mesh() const;

	};

	class Circle;
	class Polygon;
	class Rectangle;

	template<>
	bool overlaps<Extent, Extent>(const Extent& c1, const Extent& c2);

	template<>
	bool overlaps<Extent, Circle>(const Extent& c1, const Circle& c2);

	template<>
	bool overlaps<Extent, Polygon>(const Extent& c1, const Polygon& c2);

	template<>
	bool overlaps<Extent, Rectangle>(const Extent& c1, const Rectangle& c2);

	template<>
	bool contains<Extent, glm::vec2>(const Extent& c, const glm::vec2& p);

	template<>
	bool contains<Extent, Extent>(const Extent& c1, const Extent& c2);

}