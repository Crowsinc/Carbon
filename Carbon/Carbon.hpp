
#include <memory>
#include <optional>
#include <stdint.h>

namespace cbn
{

	// Typedefs
	template<typename T>
	using SPtr = std::shared_ptr<T>;

	template<typename T>
	using UPtr = std::unique_ptr<T>;

	template<typename T>
	using Opt = std::optional<T>;




	// Initializers 
    // TODO: bool InitializeGraphics(const Version& api_version, bool debug_context = false);



}


