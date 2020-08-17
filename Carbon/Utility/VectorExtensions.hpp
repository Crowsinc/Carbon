#pragma once

#include <vector>
#include <algorithm>


namespace cbn
{
	template<typename T>
	void swap_erase(std::vector<T>& container, const size_t index);

	template<typename T>
	bool remove_all(std::vector<T>& container, const T value);

	template<typename T>
	bool contains(std::vector<T>& container, const T value);

}

#include "VectorExtensions.tpp"