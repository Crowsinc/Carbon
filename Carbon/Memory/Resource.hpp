#pragma once

#include <memory>

namespace cbn
{

	template<typename T>
	using SRes = std::shared_ptr<T>;

	template<typename T>
	using URes = std::unique_ptr<T>;

	struct Resource
	{
		template<typename T>
		using Deleter = void(*)(T*);

		template<typename T>
		static SRes<T> WrapShared(T* resource);

		template<typename T>
		static URes<T> WrapUnique(T* resource);

		template<typename T, typename ...Args>
		static SRes<T> AllocateShared(Args&&... args);

		template<typename T, typename ...Args>
		static URes<T> AllocateUnique(Args&&... args);
		
	};

}

#include "Resource.tpp"