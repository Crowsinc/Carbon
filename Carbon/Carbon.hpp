
#include <memory>
#include <optional>
#include <stdint.h>

//TODO: add useful headers

namespace cbn
{

	// Typedefs
	template<typename T>
	using SPtr = std::shared_ptr<T>;

	template<typename T>
	using UPtr = std::unique_ptr<T>;

	template<typename T>
	using Opt = std::optional<T>;



}


