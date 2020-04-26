#pragma once

#include <optional>

namespace cbn
{

	template<typename T>
	class Resource
	{
	public:

		using Deleter = void(*)(T&);
	
		static Resource<T> Wrap(T& resource, const Deleter& deleter);

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

		void operator=(Resource<T>& resource) = delete;

	};

	template<typename T>
	using Res = Resource<T>;


}

#include "Resource.tpp"