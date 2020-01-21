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

		int m_NextHandleID;
		std::vector<std::pair<int,Callback>> m_Callbacks;

	public:

		template<typename ...P>
		void invoke(P... params) const;
	
		template<typename ...P>
		void operator()(P... params) const;

		int add_callback(const Callback& callback);

		int operator+=(const Callback& callback);

		void remove_callback(const int callback_handle);

		void operator-=(const int callback_handle);
	};

	
}

#include "Event.tpp"