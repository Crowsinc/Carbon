#pragma once

#include <cstdint>
#include <optional>
#include <functional>
#include <unordered_map>

#include "../String.hpp"

namespace cbn
{

	class Identifier
	{
	private:

		static std::unordered_map<std::string, uint64_t> s_AliasIDs;
		static uint64_t s_NextAliasID;

		String m_Alias;
		uint64_t m_AliasID;

		void update_alias(const String& alias);

	public:

		Identifier();

		Identifier(const char* alias);
		
		Identifier(const String& alias);

		Identifier(const Identifier& copy);

		uint64_t alias_id() const;

		const String& alias() const;

		void operator=(const char* alias);
		
		void operator=(const String& alias);

		void operator=(const Identifier& other);

		bool operator>(const Identifier& other) const;

		bool operator<(const Identifier& other) const;

		bool operator==(const Identifier& other) const;

		bool operator!=(const Identifier& other) const;

		bool operator>=(const Identifier& other) const;

		bool operator<=(const Identifier& other) const;

	};

}

namespace std
{

	template<>
	struct hash<cbn::Identifier>
	{
		std::size_t operator()(const cbn::Identifier& identifier) const
		{
			return identifier.alias_id();
		}
	};

	template<>
	struct less<cbn::Identifier>
	{
		std::size_t operator()(const cbn::Identifier& left, const cbn::Identifier& right) const
		{
			return left < right;
		}
	};

}

// Common data structures
template<typename T>
using IdentityMap = std::unordered_map<cbn::Identifier, T>;