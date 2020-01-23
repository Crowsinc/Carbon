#pragma once

#include "Algortihms.hpp"

#include <cctype>

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

	bool starts_with(const std::string_view& str, const std::string_view& term, bool ignore_whitespace)
	{
		// If we are ignoring whitespace, then trim the string otherwise just leave it
		std::string_view trimmed_view = ignore_whitespace ? left_trim(str) : str;

		// If the trimmed view is both long enough to fit our term, and
		// start with the given term, then we can return true
		return trimmed_view.size() >= term.size() &&
			trimmed_view.compare(0, term.size(), term) == 0;
	}

	//-------------------------------------------------------------------------------------

	bool ends_with(const std::string_view& str, const std::string_view& term, bool ignore_whitespace)
	{
		// If we are ignoring whitespace, then trim the string otherwise just leave it
		std::string_view trimmed_view = ignore_whitespace ? right_trim(str) : str;

		// If the trimmed view is both long enough to fit our term, and
		// start with the given term, then we can return true
		return trimmed_view.size() - term.size() > 0 &&
			trimmed_view.compare(trimmed_view.size() - term.size(), term.size(), term) == 0;
	}

	//-------------------------------------------------------------------------------------

	std::vector<std::string_view> split(const std::string_view& str, const std::string_view& delimiter)
	{
		size_t start_pointer = 0, end_pointer = str.find(delimiter);
		std::vector<std::string_view> sections;

		// Loop through the entire string finding searching for the delimiter
		while(end_pointer != std::string::npos)
		{
			// Here we have found a delimiter so split the string and add it to our sections vector
			std::string_view section = str.substr(start_pointer, end_pointer - start_pointer);

			// Only add the section if its not empty
			if(!section.empty())
			{
				sections.push_back(section);
			}

			// Update the start point to the end of the section and the delimiter
			start_pointer = end_pointer + delimiter.length();

			// Perform another search for the next delimiter
			end_pointer = str.find(delimiter, start_pointer);
		}

		// Get the final section and add it if its not empty
		std::string_view section = str.substr(start_pointer, end_pointer - start_pointer);
		if(!section.empty()) sections.push_back(section);

		return sections;
	}

	//-------------------------------------------------------------------------------------

	bool contains(const std::string_view& str, const std::string_view& term)
	{
		return str.find(term) != std::string_view::npos;
	}

	//-------------------------------------------------------------------------------------

	std::string& remove_all(std::string& str, const std::string_view& term)
	{
		size_t pointer = str.find(term);

		// Loop through the entire string finding searching for the term to remove
		while(pointer != std::string::npos)
		{
			// Here we have found an instance of the term we want to remove
			// We will erase it from the string
			str.erase(pointer, term.size());

			// Perform another search for the next term
			pointer = str.find(term, pointer);
		}

		return str;
	}

	//-------------------------------------------------------------------------------------

	std::string_view left_trim(const std::string_view& str)
	{
		// Loop through the string to find the first non-space character
		for(size_t i = 0; i < str.size(); i++)
		{
			// Check if the character is not a space
			if(!std::isspace(str[i]))
			{
				// This is not a space, so we should add 'i' as the 
				// new offset in the string_view, which will effectively
				// trim all the whitespace on the left
				return str.substr(i, str.size() - i);
			}

		}
		return str;
	}

	//-------------------------------------------------------------------------------------

	std::string_view right_trim(const std::string_view& str)
	{
		// Loop through the string backwards to find the first non-space character
		for(int64_t i = str.size() - 1; i >= 0; i--)
		{
			// Check if the character is not a space
			if(!std::isspace(str[i]))
			{
				// This is not a space, so we should add 'i' as the 
				// new offset in the string_view, which will effectively
				// trim all the whitespace on the left
				return str.substr(i);
			}

		}
		return str;
	}

	//-------------------------------------------------------------------------------------

	std::string_view trim(const std::string_view& str)
	{
		return left_trim(right_trim(str));
	}

	//-------------------------------------------------------------------------------------
}