#pragma once

#include "../Transforms/Transformable.hpp"
#include "../Physics/Collider.hpp"
#include "Mesh.hpp"

#include "Extent.hpp"
#include "Circle.hpp"
#include "Rectangle.hpp"


namespace cbn
{

	class Extent;

	class Polygon : public Collider, public Transformable<Translatable2D, Rotatable2D>
	{
	private:

		DynamicMesh m_Mesh;

	public:

		Polygon();

		Polygon(const DynamicMesh& mesh);

		Polygon(const Transform& transform, const DynamicMesh& mesh);

		void reshape(const DynamicMesh& mesh);

		glm::vec2 centroid() const;

		uint64_t sides() const;

		const Extent extent() const;

		const DynamicMesh& mesh() const;
	};

	class Circle;
	class Rectangle;

	template<>
	bool overlaps<Polygon, Polygon>(const Polygon& c1, const Polygon& c2);

	//TODO: requires line geometry functions to be implemented
	//template<>
	//bool overlaps<Polygon, Circle>(const Polygon& c1, const Circle& c2);

	template<>
	bool overlaps<Polygon, Extent>(const Polygon& c1, const Extent& c2);

	template<>
	bool overlaps<Polygon, Rectangle>(const Polygon& c1, const Rectangle& c2);

	// TODO: Implement after line geometry functions 
	//template<>
	//bool contains<Polygon, glm::vec2>(const Polygon& c, const glm::vec2& p);

}