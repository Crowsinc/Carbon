#pragma once

#include <vector>
#include <algorithm>
#include <string_view>

//TODO: figure out how to do this properly in a container non-specific way

namespace cbn
{

	// Vector Algorithms

//	template<typename T>
//	void swap_erase(T* start, T* end, const size_t index);
//
//	template<typename T>
//	bool remove_all(T* start, T* end, const T value);
//
//	template<typename T>
//	bool contains(T* start, T* end, const T value);


	template<typename T>
	void swap_erase(std::vector<T>& container, const size_t index);

	template<typename T>
	bool remove_all(std::vector<T>& container, const T value);

	template<typename T>
	bool contains(std::vector<T>& container, const T value);

	// String Algorithms

	bool starts_with(const std::string_view& str, const std::string_view& term, bool ignore_whitespace = true);

	bool ends_with(const std::string_view& str, const std::string_view& term, bool ignore_whitespace = true);

	std::vector<std::string_view> split(const std::string_view& str, const std::string_view& delimiter);

	bool contains(const std::string_view& str, const std::string_view& term);

	std::string& remove_all(std::string& str, const std::string_view& term);

	std::string_view left_trim(const std::string_view& str);

	std::string_view right_trim(const std::string_view& str);

	std::string_view trim(const std::string_view& str);

}

#include "Algorithms.tpp"