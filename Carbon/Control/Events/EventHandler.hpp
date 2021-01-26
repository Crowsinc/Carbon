#pragma once

#include "Subscription.hpp"
#include "EventCallback.hpp"
#include "../../Diagnostics/Assert.hpp"

#include <iostream>

namespace cbn
{

	template<typename Parent, typename ...Args>
	class EventHost;
	
	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	class EventHandler
	{
		friend class EventHost<Parent, Args...>;
	private:

		std::unordered_map<uint64_t, std::map<uint64_t, EventCallback<Args...>>> m_HostCallbacks;

		void register_host(const EventHost<Parent, Args...>& host);

		void unregister_host(const EventHost<Parent, Args...>& host);

		void dispatch_event(const EventHost<Parent, Args...>& host, Args... args);

		Subscription create_subscription(const EventHost<Parent, Args...>& host, EventCallback<Args...> callback);

	public:

		bool is_registered(const EventHost<Parent, Args...>& host) const;

	};

	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	inline void EventHandler<Parent, Args...>::register_host(const EventHost<Parent, Args...>& host)
	{
		m_HostCallbacks.emplace(host.uid(), std::map<uint64_t, EventCallback<Args...>>{});
	}

	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	inline void EventHandler<Parent, Args...>::unregister_host(const EventHost<Parent, Args...>& host)
	{
		m_HostCallbacks.erase(host.uid());
	}

	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	inline void EventHandler<Parent, Args...>::dispatch_event(const EventHost<Parent, Args...>& host, Args... args)
	{
		CBN_Assert(m_HostCallbacks.contains(host.uid()) == 1, "Cannot create subscription for unregistered host");

		const auto& callbacks = m_HostCallbacks.at(host.uid());
		for(const auto& [_, callback] : callbacks)
			callback.invoke(args...);
	}

	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	inline Subscription EventHandler<Parent, Args...>::create_subscription(const EventHost<Parent, Args...>& host, EventCallback<Args...> callback)
	{
		CBN_Assert(m_HostCallbacks.contains(host.uid()) == 1, "Cannot create subscription for unregistered host");

		const auto host_uid = host.uid();
		const auto callback_uid = callback.uid();

		// Add the callback and create the subscription with a hardcoded unsubscriber function
		m_HostCallbacks[host_uid].emplace(callback_uid, callback);
		return Subscription([&, host_uid, callback_uid]()
		{
			m_HostCallbacks[host_uid].erase(callback_uid);
		});
	}
	
	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	inline bool EventHandler<Parent, Args...>::is_registered(const EventHost<Parent, Args...>& host) const
	{
		return m_HostCallbacks.contains(host.uid());
	}

	//-------------------------------------------------------------------------------------

}