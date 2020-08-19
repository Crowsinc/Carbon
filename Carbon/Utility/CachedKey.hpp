#pragma once

#include <functional>


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

		void operator==(const CachedKey<T>& other) const
		{
			return hash() == other.hash();
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

}

namespace std
{

	template<typename T>
	struct hash<cbn::CachedKey<T>>
	{
		std::size_t operator()(const cbn::CachedKey<T>& key)
		{
			return key.hash();
		}
	};

	template<typename T>
	struct less<cbn::CachedKey<T>>
	{
		std::size_t operator()(const cbn::CachedKey<T>& left, const cbn::CachedKey<T>& right)
		{
			return left.hash() < right.hash();
		}
	};

}