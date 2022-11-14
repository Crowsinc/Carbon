#pragma once

#include "../Transforms/Transformable.hpp"
#include "../Physics/Collider.hpp"

#include "Extent.hpp"
#include "Polygon.hpp"
#include "Rectangle.hpp"

namespace cbn
{
	class Extent;

	class Circle : public Collider, public Transformable<Translatable2D>
	{
	private:

		float m_Radius;

	public:

		Circle();

		Circle(const float radius);

		Circle(const Transform& transform, const float radius);

		void resize(const float radius);

		float radius() const;

		const glm::vec2& centre() const;

		Extent extent() const;
	};

	class Polygon;
	class Rectangle;

	template<>
	bool overlaps<Circle, Circle>(const Circle& c1, const Circle& c2);

	template<>
	bool overlaps<Circle, Extent>(const Circle& c1, const Extent& c2);

	//TODO: uncomment after polygon-circle overlap test is made in Polygon.cpp
	//template<>
	//bool overlaps<Circle, Polygon>(const Circle& c1, const Polygon& c2);

	template<>
	bool overlaps<Circle, Rectangle>(const Circle& c1, const Rectangle& c2);

	template<>
	bool contains<Circle, glm::vec2>(const Circle& c, const glm::vec2& p);

}