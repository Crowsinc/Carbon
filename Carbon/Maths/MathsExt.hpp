#pragma once

#include <cmath>

namespace cbn
{

	template<typename T>
	T to_degrees(const T radians);

	template<typename T>
	T to_radians(const T degrees);

}

// Template implementation
#include "MathsExt.tpp"