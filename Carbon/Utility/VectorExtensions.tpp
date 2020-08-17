#pragma once

#include "VectorExtensions.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	template<typename T> 
	void swap_erase(std::vector<T>& container, const size_t index)
	{
		// Swap the element we want to remove with the last element in the container, then remove it.
		// We do this to force the erase operation to run in O(1) time as it doesn't have to shuffle elements down
		std::swap(container[index], container[container.size() - 1]);
		container.pop_back();
	}

	//-------------------------------------------------------------------------------------

	template<typename T>
	bool remove_all(std::vector<T>& container, const T value)
	{
		// Iterate through all elements in the vector and remove all which match the value
		for(int i = 0; i < container.size(); i++)
		{
			if(container[i] == value)
			{
				// Erase the element and decrement the 
				// iterator to avoid skipping over an element
				container.erase(container.begin() + i);
				i--;
			}
		}
	}

	//-------------------------------------------------------------------------------------

	template<typename T>
	bool contains(std::vector<T>& container, const T value)
	{
		return std::find(container.begin(),container.end(),value) != container.end();
	}

	//-------------------------------------------------------------------------------------
	
}