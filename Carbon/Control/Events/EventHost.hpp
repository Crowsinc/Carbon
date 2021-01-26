#pragma once

#include <iostream>

#include <map>
#include <functional>
#include <unordered_map>

#include "../../Data/Identity/Unique.hpp"
#include "../../Diagnostics/Assert.hpp"

#include "Subscription.hpp"
#include "EventHandler.hpp"

namespace cbn
{

	// The EventHandler is static and manages all events of a type. The EventHost is the interface to the events 
	// from a particular object. So the user interfaces with a specific instance, but each host interfaces with 
	// a static event handler, removing all event & subscription lifetime issues. 

	template<typename Parent, typename ...Args>
	class EventHandler;

	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	class EventHost : public Unique<EventHost<Parent, Args...>>
	{
		friend Parent;
	public:

		static EventHandler<Parent, Args...> EventHandler;

	private:

		void dispatch(Args... args) const;

	public:

		~EventHost();

		Subscription subscribe(typename EventCallback<Args...>::RawType callback);

	};

	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	EventHandler<Parent, Args...> EventHost<Parent, Args...>::EventHandler{};

	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	inline void EventHost<Parent, Args...>::dispatch(Args... args) const
	{
		// Only dispatch if the host has actually be registered
		if(EventHandler.is_registered(*this))
			EventHandler.dispatch_event(*this, args...);
	}

	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	inline EventHost<Parent, Args...>::~EventHost()
	{
		// If the event host was registered, then unregister it
		if(EventHandler.is_registered(*this))
			EventHandler.unregister_host(*this);
	}

	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	inline Subscription EventHost<Parent, Args...>::subscribe(typename EventCallback<Args...>::RawType callback)
	{
		// We want to register on the first subscription so we aren't wasting time
		// for events that aren't even being used. This gives a performance boost as 
		// there will likely be thousands of objects whose events wouldn't be used in
		// a game scenario.
		if(!EventHandler.is_registered(*this))
			EventHandler.register_host(*this);

		return EventHandler.create_subscription(*this, EventCallback<Args...>{callback});
	}
	
	//-------------------------------------------------------------------------------------

}

