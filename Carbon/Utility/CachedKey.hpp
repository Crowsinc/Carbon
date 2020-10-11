#pragma once

#include <functional>

#include <iostream>

namespace cbn
{

	template<typename T>
	class CachedKey
	{
	private:

		T m_Key;
		std::size_t m_Hash;

		void update_key(const T& key)
		{
			m_Key = key;
			m_Hash = std::hash<T>{}(key);
		}

	public:

		CachedKey() = default;

		CachedKey(const T& key)
		{
			update_key(key);
		}

		void operator=(const T key)
		{
			update_key(key);
		}

		void operator=(const T& key)
		{
			update_key(key);
		}

		bool operator==(const CachedKey<T>& other) const
		{
		//	return hash() == other.hash();
			return m_Key == other.m_Key; // This may drop performance, but is required
		}

		const std::size_t& hash() const
		{
			return m_Hash;
		}

		const T& value() const 
		{ 
			return m_Key; 
		}

		operator const T& () const
		{
			return value();
		}

	};

	template<typename T>
	using CKey = CachedKey<T>;
	using Name = CKey<std::string>;

}

namespace std
{

	template<typename T>
	struct hash<cbn::CachedKey<T>>
	{
		std::size_t operator()(const cbn::CachedKey<T>& key) const
		{
			return key.hash();
		}
	};

	template<typename T>
	struct less<cbn::CachedKey<T>>
	{
		std::size_t operator()(const cbn::CachedKey<T>& left, const cbn::CachedKey<T>& right) const
		{
			return left.hash() < right.hash();
		}
	};

}