#include "Resource.hpp"

#include "../Diagnostics/Assert.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	template<typename T>
	inline Resource<T> Resource<T>::Wrap(T& resource, const Deleter& deleter)
	{
		// Create a null resource, and then initialize it
		Res<T> resource_wrapper;
		resource_wrapper.m_Deleter = deleter;
		resource_wrapper.m_Resource = std::move(resource);
		return resource_wrapper;
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T>
	constexpr Resource<T>::Resource()
		: m_Deleter(nullptr) {}
	
	//-------------------------------------------------------------------------------------

	template<typename T>
	constexpr Resource<T>::Resource(std::nullptr_t) 
		: m_Deleter(nullptr) {}
	
	//-------------------------------------------------------------------------------------

	template<typename T>
	Resource<T>::~Resource()
	{
		// If the resource exists, then run the deleter on it. Note that
		// the deleter will not run for moved resources because the resource
		// will be freed from the optional during the move. 
		if(m_Resource.has_value() && m_Deleter != nullptr)
		{
			m_Deleter(m_Resource.value());
		}
	}

	//-------------------------------------------------------------------------------------

	template<typename T>
	Resource<T>::Resource(Resource<T>&& resource) 
		: m_Deleter(resource.m_Deleter)
	{
		m_Resource = std::move(resource.m_Resource.value());

		// Reset the other resources' optional resource. This will run
		// the destructor for the resource T, but it will stop the deleter
		// for T from running. Therefore all deletion with T, should be 
		// handled by the deleter not T's destructor.
		resource.m_Resource.reset();
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T>
	inline bool Resource<T>::exists() const
	{
		return m_Resource.has_value();
	}

	//-------------------------------------------------------------------------------------

	template<typename T>
	T& Resource<T>::get()
	{
		CBN_Assert(exists(), "Cannot return uninitialised resource");

		return m_Resource.value();
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T>
	Resource<T>::operator T& ()
	{
		CBN_Assert(exists(), "Cannot return uninitialised resource");

		return m_Resource.value();
	}

	//-------------------------------------------------------------------------------------

	template<typename T>
	inline Resource<T>::operator const T& () const
	{
		CBN_Assert(exists(), "Cannot return uninitialised resource");

		return m_Resource.value();
	}

	//-------------------------------------------------------------------------------------

	template<typename T>
	inline Resource<T>::operator const bool() const
	{
		return exists();
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T>
	const T* Resource<T>::operator->() const
	{
		CBN_Assert(exists(), "Cannot return uninitialised resource");

		return &m_Resource.value();
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T>
	inline T* Resource<T>::operator->()
	{
		CBN_Assert(exists(), "Cannot return uninitialised resource");
		
		return &m_Resource.value();
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T>
	inline void Resource<T>::operator=(Resource<T>&& resource)
	{
		// If we are already wrapping a resource, then we need to properly delete it
		if(m_Resource.has_value() && m_Deleter != nullptr)
		{
			m_Deleter(m_Resource.value());
		}

		// Now move the resource over to this instance
		m_Deleter = resource.m_Deleter;
		m_Resource = std::move(resource.m_Resource);

		// Reset the other resources' optional resource. This will run
		// the destructor for the resource T, but it will stop the deleter
		// for T from running. Therefore all deletion with T, should be 
		// handled by the deleter not T's destructor.
		resource.m_Resource.reset();
	}

	//-------------------------------------------------------------------------------------

}