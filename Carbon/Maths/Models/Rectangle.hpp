#pragma once

#include "../Transforms/Transformable.hpp"
#include "../Physics/Collider.hpp"
#include "Mesh.hpp"

#include "Extent.hpp"
#include "Circle.hpp"
#include "Polygon.hpp"

namespace cbn
{

	class Extent;

	class Rectangle : public Collider, public Transformable<Translatable2D, Rotatable2D>
	{
	private:

		glm::vec2 m_Size;

	public:

		Rectangle();

		Rectangle(const glm::vec2 size);

		Rectangle(const Transform& transform, const glm::vec2 size);

		void resize(const glm::vec2 size);

		glm::vec2 transform_to_local(const glm::vec2 point) const;

		std::tuple<glm::vec2, glm::vec2> axes() const;

		const glm::vec2& centre() const;

		const glm::vec2& size() const;

		StaticMesh<4> mesh() const;

		Extent extent() const;
	};

	class Circle;
	class Polygon;

	template<>
	bool overlaps<Rectangle, Rectangle>(const Rectangle& c1, const Rectangle& c2);

	template<>
	bool overlaps<Rectangle, Circle>(const Rectangle& c1, const Circle& c2);

	template<>
	bool overlaps<Rectangle, Extent>(const Rectangle& c1, const Extent& c2);

	template<>
	bool overlaps<Rectangle, Polygon>(const Rectangle& c1, const Polygon& c2);

	template<>
	bool contains<Rectangle, glm::vec2>(const Rectangle& c, const glm::vec2& p);

	//TODO: contains rectangle

}