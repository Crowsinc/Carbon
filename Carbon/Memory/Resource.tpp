#include "Resource.hpp"

#include <utility>

#include "../Diagnostics/Assert.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	template<typename T>
	SRes<T> Resource::WrapShared(T* resource)
	{
		return URes<T>(resource);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T>
	URes<T> Resource::WrapUnique(T* resource)
	{
		return URes<T>(resource);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T, typename ...Args>
	SRes<T> Resource::AllocateShared(Args&& ...args)
	{
		return SRes<T>(new T(args...));
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T, typename ...Args>
	URes<T> Resource::AllocateUnique(Args&& ...args)
	{
		return URes<T>(new T(args...));
	}

	//-------------------------------------------------------------------------------------

}

