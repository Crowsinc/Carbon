#pragma once

#include <map>
#include <functional>
#include <unordered_map>

#include "../../Data/Identity/Identified.hpp"
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
	class EventHost : public Identified
	{
		friend Parent;
	public:

		static EventHandler<Parent, Args...> EventHandler;

	private:

		void dispatch(Args... args) const;

	public:

		EventHost();

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
		EventHandler.dispatch_event(*this, args...);
	}

	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	inline EventHost<Parent, Args...>::EventHost()
	{
		EventHandler.register_host(*this);
	}

	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	inline EventHost<Parent, Args...>::~EventHost()
	{
		EventHandler.unregister_host(*this);
	}

	//-------------------------------------------------------------------------------------

	template<typename Parent, typename ...Args>
	inline Subscription EventHost<Parent, Args...>::subscribe(typename EventCallback<Args...>::RawType callback)
	{
		return EventHandler.create_subscription(*this, EventCallback<Args...>{callback});
	}

	//-------------------------------------------------------------------------------------

}

