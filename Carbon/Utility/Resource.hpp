#pragma once

#include <optional>

namespace cbn
{
	
	//TODO: automate the resource method. That is, force resources to implement it properly
#define RAIIResource(x) friend class Resource<x>; static void destroy(x&)

	template<typename T>
	class Resource
	{
	public:

		using Deleter = void(*)(T&);
	
		static Resource<T> Wrap(T&& resource, const Deleter& deleter);

	private:

		std::optional<T> m_Resource;
		Deleter m_Deleter;

	public:

		constexpr Resource();

		Resource(Resource<T>&& resource);

		constexpr Resource(std::nullptr_t);

		Resource(const Resource<T>& resource) = delete;

		~Resource();

		bool exists() const;

		T& get();

		operator T& ();

		operator const T& () const;

		operator const bool() const;

		const T* operator->() const;

		T* operator->();

		void operator=(Resource<T>&& resource);

	};

	template<typename T>
	using Res = Resource<T>;


}

#include "Resource.tpp"