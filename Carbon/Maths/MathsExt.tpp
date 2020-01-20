#include <type_traits>
#include <cmath>


namespace cbn
{
	//-------------------------------------------------------------------------------------

	constexpr float PI = 3.14159265358979323846f;

	//-------------------------------------------------------------------------------------

	template<typename T>
	inline T to_degrees(const T radians)
	{
		static_assert(std::is_arithmetic<T>::value, "Cannot convert non-arithmetic type to degrees");
		return (radians * 180) / PI;
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T>
	inline T to_radians(const T degrees)
	{
		static_assert(std::is_arithmetic<T>::value, "Cannot convert non-arithmetic type to radians");
		return (degrees * PI) / 180;
	}
	
	//-------------------------------------------------------------------------------------

}