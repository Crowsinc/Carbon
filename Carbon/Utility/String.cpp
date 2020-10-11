#include "String.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	String::String(const char* data)
		: m_Data(data) {}

	//-------------------------------------------------------------------------------------


	String::String(const std::string_view& data)
		: m_Data(data) {}

	//-------------------------------------------------------------------------------------

	String::String(const std::string& other)
		: m_Data(other) {}

	//-------------------------------------------------------------------------------------

	String::String(const String& other)
		: m_Data(other.m_Data) {}

	//-------------------------------------------------------------------------------------

	String::String(std::string&& other)
		: m_Data(other) {}
	
	//-------------------------------------------------------------------------------------

	String::String(String&& other)
		: m_Data(other.m_Data) {}

	//-------------------------------------------------------------------------------------

	bool String::starts_with(const std::string_view & term, bool ignore_whitespace) const
	{
		// If we are ignoring whitespace, then trim the string otherwise just leave it
		String trimmed_view = ignore_whitespace ? left_trim() : String{m_Data};

		// If the trimmed view is both long enough to fit our term, and
		// start with the given term, then we can return true
		return trimmed_view.size() >= term.size() &&
			trimmed_view.m_Data.compare(0, term.size(), term) == 0;
	}
	
	//-------------------------------------------------------------------------------------

	bool String::ends_with(const std::string_view& term, bool ignore_whitespace) const
	{
		// If we are ignoring whitespace, then trim the string otherwise just leave it
		String trimmed_view = ignore_whitespace ? right_trim() : String{m_Data};

		// If the trimmed view is both long enough to fit our term, and
		// start with the given term, then we can return true
		return trimmed_view.size() - term.size() > 0 &&
			trimmed_view.m_Data.compare(trimmed_view.size() - term.size(), term.size(), term) == 0;
	}

	//-------------------------------------------------------------------------------------

	std::vector<String> String::split(const std::string_view& delimiter) const
	{
		size_t start_pointer = 0, end_pointer = find(delimiter);
		std::vector<String> sections;

		// Loop through the entire string finding searching for the delimiter
		while(end_pointer != std::string::npos)
		{
			// Here we have found a delimiter so split the string and add it to our sections vector
			String section = m_Data.substr(start_pointer, end_pointer - start_pointer);

			// Only add the section if its not empty
			if(!section.is_empty())
			{
				sections.push_back(section);
			}

			// Update the start point to the end of the section and the delimiter
			start_pointer = end_pointer + delimiter.length();

			// Perform another search for the next delimiter
			end_pointer = find(delimiter, start_pointer);
		}

		// Get the final section and add it if its not empty
		String section = m_Data.substr(start_pointer, end_pointer - start_pointer);
		if(!section.is_empty()) sections.push_back(section);

		return sections;
	}

	//-------------------------------------------------------------------------------------

	size_t String::find(const std::string_view& term, const size_t offset) const
	{
		return m_Data.find(term, offset);
	}
	
	//-------------------------------------------------------------------------------------

	size_t String::find_last(const std::string_view& term) const
	{
		return m_Data.rfind(term);
	}
	
	//-------------------------------------------------------------------------------------

	void String::replace_all(const std::string_view& term, const std::string& replacement)
	{
		size_t pointer = find(term);

		// Loop through the entire string finding searching for the term to remove
		while(pointer != std::string::npos)
		{
			// Here we have found an instance of the term we want to remove
			// Erase the term from the string, and insert the new term
			remove(pointer, term.size());
			insert(pointer, replacement);

			// Perform another search for the next term
			pointer = find(term, pointer);
		}
	}
	
	//-------------------------------------------------------------------------------------

	void String::replace_all(const char term, const char replacement)
	{
		for(char& c : m_Data)
		{
			if(c == term)
				c = replacement;
		}
	
	}
	
	//-------------------------------------------------------------------------------------

	String String::extract(const size_t start, const size_t amount)
	{
		return m_Data.substr(start, amount);
	}
	
	//-------------------------------------------------------------------------------------

	void String::remove(const size_t start, const size_t amount)
	{
		m_Data.erase(start, amount);
	}
	
	//-------------------------------------------------------------------------------------

	void String::remove_all(const std::string_view& term)
	{
		size_t pointer = find(term);

		// Loop through the entire string finding searching for the term to remove
		while(pointer != std::string::npos)
		{
			// Here we have found an instance of the term we want to remove
			// We will erase it from the string
			remove(pointer, term.size());

			// Perform another search for the next term
			pointer = find(term, pointer);
		}
	}

	//-------------------------------------------------------------------------------------

	void String::insert(const size_t index, const std::string& term)
	{
		m_Data.insert(index, term);
	}
	
	//-------------------------------------------------------------------------------------

	void String::insert(const size_t index, const char term)
	{
		m_Data.insert(index, 1, term);
	}
	
	//-------------------------------------------------------------------------------------

	bool String::contains(const std::string_view& term, const size_t offset) const
	{
		return find(term, offset) != std::string::npos;
	}
	
	//-------------------------------------------------------------------------------------

	String String::right_trim() const
	{
		// Loop through the string backwards to find the first non-space character
		for(int64_t i = m_Data.size() - 1; i >= 0; i--)
		{
			// Check if the character is not a space
			if(!std::isspace(m_Data[i]))
			{
				// This is not a space, so we should add 'i' as the 
				// new offset in the string_view, which will effectively
				// trim all the whitespace on the left
				return String(m_Data.substr(i));
			}

		}
		return m_Data;
	}
	
	//-------------------------------------------------------------------------------------

	String String::left_trim() const
	{
		// Loop through the string to find the first non-space character
		for(size_t i = 0; i < m_Data.size(); i++)
		{
			// Check if the character is not a space
			if(!std::isspace(m_Data[i]))
			{
				// This is not a space, so we should add 'i' as the 
				// new offset in the string_view, which will effectively
				// trim all the whitespace on the left
				auto a = String(m_Data.substr(i, m_Data.size() - i));
				return a;
			}

		}
		return m_Data;
	}
	
	//-------------------------------------------------------------------------------------

	String String::trim() const
	{
		return right_trim().left_trim();
	}
	//-------------------------------------------------------------------------------------

	void String::clear()
	{
		m_Data.clear();
	}
	
	//-------------------------------------------------------------------------------------

	size_t String::size()
	{
		return m_Data.size();
	}
	
	//-------------------------------------------------------------------------------------

	bool String::is_empty()
	{
		return m_Data.empty();
	}
	
	//-------------------------------------------------------------------------------------

	const char* String::as_array()
	{
		return m_Data.c_str();
	}
	
	//-------------------------------------------------------------------------------------

	std::string::iterator String::begin()
	{
		return m_Data.begin();
	}
	
	//-------------------------------------------------------------------------------------

	std::string::const_iterator String::begin() const
	{
		return m_Data.begin();
	}
	
	//-------------------------------------------------------------------------------------

	std::string::iterator String::end()
	{
		return m_Data.end();
	}
	
	//-------------------------------------------------------------------------------------

	std::string::const_iterator String::end() const
	{
		return m_Data.end();
	}
	
	//-------------------------------------------------------------------------------------

	void String::operator=(const char* other)
	{
		m_Data = other;
	}
	
	//-------------------------------------------------------------------------------------

	void String::operator=(const std::string& other)
	{
		m_Data = other;
	}
	
	//-------------------------------------------------------------------------------------

	bool String::operator>(const std::string& other) const
	{
		return m_Data > other;
	}
	//-------------------------------------------------------------------------------------

	bool String::operator>=(const std::string& other) const
	{
		return m_Data >= other;
	}
	
	//-------------------------------------------------------------------------------------

	bool String::operator<=(const std::string& other) const
	{
		return m_Data <= other;
	}
	
	//-------------------------------------------------------------------------------------

	bool String::operator<(const std::string& other) const
	{
		return m_Data < other;
	}
	
	//-------------------------------------------------------------------------------------

	bool String::operator==(const std::string& other) const
	{
		return m_Data == other;
	}
	
	//-------------------------------------------------------------------------------------

	bool String::operator!=(const std::string& other) const
	{
		return m_Data != other;
	}
	
	//-------------------------------------------------------------------------------------

	String String::operator+(const std::string& other) const
	{
		return m_Data + other;
	}

	//-------------------------------------------------------------------------------------

	String String::operator+(const String& other) const
	{
		return m_Data + other.m_Data;
	}
	
	//-------------------------------------------------------------------------------------

	String String::operator+(const char* other) const
	{
		return m_Data + other;
	}
	
	//-------------------------------------------------------------------------------------

	String String::operator+(const int number) const
	{
		return m_Data + std::to_string(number);
	}
	
	//-------------------------------------------------------------------------------------

	void String::operator+=(const char other)
	{
		m_Data += other;
	}
	
	//-------------------------------------------------------------------------------------

	void String::operator+=(const std::string& other) 
	{
		m_Data += other;
	}

	//-------------------------------------------------------------------------------------

	char& String::operator[](size_t index) 
	{
		return m_Data[index];
	}

	//-------------------------------------------------------------------------------------

	String::operator std::string& ()
	{
		return m_Data;
	}

	//-------------------------------------------------------------------------------------

	String::operator std::string_view()
	{
		return std::string_view(m_Data);
	}

	//-------------------------------------------------------------------------------------

	String::operator const std::string&() const
	{
		return m_Data;
	}

	//-------------------------------------------------------------------------------------

	std::string* String::operator->()
	{
		return &m_Data;
	}

	//-------------------------------------------------------------------------------------

}