#pragma once

#include "../Transforms/Transformable.hpp"
#include "../Physics/Collider.hpp"

namespace cbn
{
	
	class Ray : public Collider, public Transformable<Translatable2D, Rotatable2D>
	{
	private:

		glm::vec2 m_Origin, m_Direction;

	public:

		Ray(const glm::vec2 origin, const glm::vec2 towards);

		Ray(const glm::vec2 origin, const float rotation_degrees);

		void point_towards(const glm::vec2 towards);

		const glm::vec2& direction() const;

		const glm::vec2& normal() const;

		const glm::vec2& origin() const;
	};

	class Line : public Collider
	{
	private:

		glm::vec2 m_P1, m_P2;

	public:

		Line();

		Line(const glm::vec2 p1, const glm::vec2 p2);

		void reshape(const glm::vec2 p1, const glm::vec2 p2);

		std::tuple<glm::vec2, glm::vec2> points() const;

		float gradient() const;

	};

	class Segment : public Collider
	{
	private:

		glm::vec2 m_P1, m_P2;

	public:

		Segment();

		Segment(const glm::vec2 p1, const glm::vec2 p2);

		void reshape(const glm::vec2 p1, const glm::vec2 p2);

		std::tuple<glm::vec2, glm::vec2> points() const;

		glm::vec2 normal() const;

	};

	// TODO: implement

	
	//template<>
	//bool overlaps<Line, Line>(const Line& c1, const Line& c2);

	//template<>
	//bool overlaps<Ray, Ray>(const Ray& c1, const Ray& c2);

	//template<>
	//bool overlaps<Segment, Segment>(const Segment& c1, const Segment& c2);


	//template<>
	//bool overlaps<Ray, Line>(const Ray& c1, const Line& c2);

	//template<>
	//bool overlaps<Line, Ray>(const Line& c1, const Ray& c2);


	//template<>
	//bool overlaps<Ray, Segment>(const Ray& c1, const Segment& c2);

	//template<>
	//bool overlaps<Segment, Ray>(const Segment& c1, const Ray& c2);


	//template<>
	//bool overlaps<Segment, Line>(const Segment& c1, const Line& c2);

	//template<>
	//bool overlaps<Line, Segment>(const Line& c1, const Segment& c2);


	//template<>
	//bool contains<Ray, glm::vec2>(const Ray& c, const glm::vec2& p);
	
	//template<>
	//bool contains<Line, glm::vec2>(const Line& c, const glm::vec2& p);

	//template<>
	//bool contains<Segment, glm::vec2>(const Segment& c, const glm::vec2& p);

}