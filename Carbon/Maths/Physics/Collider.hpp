#pragma once

#include <type_traits>

namespace cbn
{
	template<typename C1, typename C2>
	bool overlaps(const C1& c1, const C2& c2);

	template<typename C1, typename C2>
	bool contains(const C1& c1, const C2& c2);

	struct Collider
	{

		//TODO: fix this so that it can call its own correct contains and collides function
		/*
		template<typename C>
		bool collides(const C& other) const
		{
			return overlaps<auto>(this, other);
		}
		
		template<typename C>
		bool contains(const C& other) const
		{
			return contains<auto>(this, other);
		}
		*/

	};

}
