#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace cbn
{

	//TODO: make an equivalent to to_string
	class String
	{
	private:

		std::string m_Data;

	public:

		String() = default;

		String(const char* data);

		String(const std::string_view& other);

		String(const std::string& other);

		String(const String& other);

		String(std::string&& data);

		String(String&& data);

		bool starts_with(const std::string_view& term, bool ignore_whitespace = true) const;

		bool ends_with(const std::string_view& term, bool ignore_whitespace = true) const;

		std::vector<String> split(const std::string_view& delimiter) const;

		size_t find(const std::string_view& term, const size_t offset = 0) const;

		size_t find_last(const std::string_view& term) const;

		bool contains(const std::string_view& term,  const size_t offset = 0) const;

		void replace_all(const std::string_view& term, const std::string& replacement);

		void replace_all(const char term, const char replacement);
		
		String extract(const size_t start, const size_t amount);

		void remove(const size_t start, const size_t amount);

		void remove_all(const std::string_view& term);

		void insert(const size_t index, const std::string& term);
		
		void insert(const size_t index, const char term);

		String right_trim() const;

		String left_trim() const;

		String trim() const;

		void clear();

		size_t size();

		bool is_empty();

		const char* as_array() const;

		std::string as_std_string() const;

		std::string::iterator begin();

		std::string::const_iterator begin() const;

		std::string::iterator end();

		std::string::const_iterator end() const;

		void operator=(const char* other);

		void operator=(const String& other);

		void operator=(const std::string& other);

		bool operator>(const std::string& other) const;

		bool operator<(const std::string& other) const;

		bool operator>=(const std::string& other) const;

		bool operator<=(const std::string& other) const;
		
		bool operator==(const std::string& other) const;

		bool operator!=(const std::string& other) const;

		String operator+(const std::string& other) const;

		String operator+(const String& other) const;

		String operator+(const char* other) const;

		String operator+(const int number) const;
		
		void operator+=(const char other);

		void operator+=(const std::string& other);
		
		char& operator[](size_t index);

		operator std::string_view();

		operator const std::string&() const;

		operator std::string&();

		std::string* operator->();

	};

}

// global overloads

cbn::String operator+(const char* left, const cbn::String right);

std::ostream& operator<<(std::ostream& stream, const cbn::String str);

namespace std
{

	template<>
	struct hash<cbn::String>
	{
		std::size_t operator()(const cbn::String& string) const
		{
			return std::hash<std::string>{}(string.as_std_string());
		}
	};

	template<>
	struct less<cbn::String>
	{
		std::size_t operator()(const cbn::String& left, const cbn::String& right) const
		{
			return left < right;
		}
	};

}
