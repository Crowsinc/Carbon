#pragma once

#include <memory>
#include <optional>
#include <stdint.h>

namespace cbn
{

	template<typename T>
	using SPtr = std::shared_ptr<T>;

	template<typename T>
	using Ptr = std::unique_ptr<T>;

	template<typename T>
	using Opt = std::optional<T>;

}