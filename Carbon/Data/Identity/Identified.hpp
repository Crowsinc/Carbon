#pragma once

#include <cstdint>
#include <variant>
#include <optional>
#include <unordered_map>

#include "../String.hpp"
#include "Identifier.hpp"

namespace cbn
{

	//TODO: convert to templated class so that we can get independent identities for different class types
	// But then default it to some null type so we can use it as with before as well

	class Identified
	{
	private:
		 
		static uint64_t s_NextUID;

		uint64_t m_UID;

	public:
		
		Identified();

		Identified(const Identified& other);

		Identified(Identified&& other);

		uint64_t uid() const;

	};

}