#pragma once

#include <functional>

//TODO #include "EventHandler"

namespace cbn
{

	template<typename Parent, typename ...Args>
	class EventHandler;

	class Subscription
	{
		template<typename Parent, typename ...Args> friend class EventHandler;
	private:

		std::function<void()> m_UnsubscribeFunction;
		bool m_Subscribed;

		Subscription(std::function<void()> unsubscriber);

	public:

		Subscription();

		~Subscription();

		Subscription(Subscription&& other);

		Subscription(const Subscription& copy) = delete;

		void operator=(Subscription&& other);

		bool is_subscribed() const;

		void unsubscribe();
	};

}