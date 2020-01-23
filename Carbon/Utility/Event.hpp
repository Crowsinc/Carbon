#pragma once

#include <functional>

namespace cbn
{

	template<typename ...Args>
	class Event
	{
	public:
		
		using Callback = std::function<void(Args...)>;

	private:

		int m_NextSubscriptionID;
		std::vector<std::pair<int,Callback>> m_Callbacks;

	public:

		template<typename ...P>
		void invoke(P... params) const;
	
		template<typename ...P>
		void operator()(P... params) const;

		int subscribe(const Callback& callback);

		void unsubscribe(const int subscription_id);

	};

	
}

#include "Event.tpp"