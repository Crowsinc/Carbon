#pragma once

#include <type_traits>

namespace cbn
{

	//-------------------------------------------------------------------------------------

	template<typename E>
	constexpr auto value(E e)
	{
		return static_cast<std::underlying_type_t<E>>(e);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename E>
	constexpr auto to_enum(std::underlying_type_t<E> v)
	{
		return static_cast<E>(v);
	}

	//-------------------------------------------------------------------------------------

	template<typename E>
	constexpr auto type(E e)
	{
		return std::underlying_type_t<E>;
	}

	//-------------------------------------------------------------------------------------


}