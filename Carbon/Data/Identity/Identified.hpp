#pragma once

#include <cstdint>
#include <variant>
#include <optional>
#include <unordered_map>

#include "../String.hpp"
#include "Identifier.hpp"

namespace cbn
{

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