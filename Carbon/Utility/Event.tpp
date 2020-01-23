#include "Event.hpp"

#include <algorithm>

#include "../Diagnostics/Assert.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	template<typename ...Args>
	template<typename ...P>
	inline void Event<Args...>::invoke(P... params) const
	{
		// Invoke all the subscriber's callbacks
		for(const auto callback : m_Callbacks)
		{
			callback.second(params...);
		}
	}
	
	//-------------------------------------------------------------------------------------

	template<typename ...Args>
	template<typename ...P>
	inline void Event<Args...>::operator()(P... params) const
	{
		invoke(params...);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename ...Args>
	inline int Event<Args...>::subscribe(const Callback& callback)
	{
		// Create a new ID for this subscriber, then insert pack it 
		// into the callback vector along with the callback. 
		const int subscription_id = m_NextSubscriptionID++;
		m_Callbacks.push_back(std::make_pair(subscription_id,callback));
		return subscription_id;
	}
	
	//-------------------------------------------------------------------------------------

	template<typename ...Args>
	inline void Event<Args...>::unsubscribe(const int subscription_id)
	{
		// Simply search for the pair with the given handle in the 
		// callback vector. This is slow, but it shouldnt matter 
		// since the unsuscribe operation should not commonly be used. 
		for(int i = 0; i < m_Callbacks.size(); i++)
		{
			if(m_Callbacks[i].first == subscription_id)
			{
				// Swap the callback with the last callback, then erase it
				// in order to prevent unnecessary shuffling
				std::swap(m_Callbacks[i], m_Callbacks[m_Callbacks.size() - 1]);
				m_Callbacks.erase(m_Callbacks.end());
				return;
			}
		}
		CBN_Assert(false, "Subscription ID is not associated with any subscriptions in this event");
	}
	
	//-------------------------------------------------------------------------------------

}