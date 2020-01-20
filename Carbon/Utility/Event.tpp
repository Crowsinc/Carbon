#include "Event.hpp"

#include <algorithm>

#include "Assert.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	template<typename ...T>
	inline void Event<T...>::invoke(T ... params) const
	{
		for(const auto callback : m_Callbacks)
		{
			callback(params...);
		}
	}
	
	//-------------------------------------------------------------------------------------

	template<typename ...T>
	inline void Event<T...>::operator()(T ... params) const
	{
		invoke(params...);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename ...T>
	inline int Event<T...>::add_callback(const Callback& callback)
	{
		// Create a new handle for the callback then insert it into the
		// callback vector along with the callback. 
		const int new_handle = m_NextHandleID++;
		m_Callbacks.push_back(std::make_pair(new_handle,callback));
		return new_handle;
	}
	
	//-------------------------------------------------------------------------------------

	template<typename ...T>
	inline int Event<T...>::operator+=(const Callback& callback)
	{
		return add_callback(callback);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename ...T>
	inline void Event<T...>::remove_callback(const int callback_handle)
	{
		// Simply search for the pair with the given handle in the 
		// callback vector. This is slow, but it shouldnt matter 
		// since the remove callback operation should not commonly be used. 
		for(int i = 0; i < m_Callbacks.size(); i++)
		{
			if(m_Callbacks[i].first == callback_handle)
			{
				// Swap the callback with the last callback, then erase it
				// in order to prevent unnecessary shuffling
				std::swap(m_Callbacks[i], m_Callbacks[m_Callbacks.size() - 1));
				m_Callbacks.erase(m_Callbacks.end());
				return;
			}
		}
		CBN_Assert(false, "Callback handle is not associated with any callbacks in this event");
	}
	
	//-------------------------------------------------------------------------------------

	template<typename ...T>
	inline void Event<T...>::operator-=(const int callback_handle)
	{
		remove_callback(callback_handle);
	}

	//-------------------------------------------------------------------------------------

}