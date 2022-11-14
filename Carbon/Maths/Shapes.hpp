#pragma once

namespace cbn
{

	template<typename T>
	struct Rect
	{
		T x, y, width, height;
	};

	template<typename T>
	bool intersects(const Rect<T> a, const Rect<T> b)
	{
		return !(a.x >= b.x + b.width || a.x + a.width <= b.x || a.y >= b.y + b.height || a.y + a.height <= b.y);
	}

	template<typename T>
	bool contains(const Rect<T> outer, const Rect<T> inner)
	{
		return outer.x <= inner.x && outer.y <= inner.y && outer.x + outer.width >= inner.x + inner.width && outer.y + outer.height >= inner.y + inner.height;
	}

}