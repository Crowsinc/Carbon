#pragma once

#include <functional>

namespace cbn
{

	template<typename ...T>
	class Event
	{
	public:
		
		using Callback = std::function<void(T...)>;

	private:

		int m_NextHandleID;
		std::vector<std::pair<int,Callback>> m_Callbacks;

	public:

		void invoke(T... params) const;
	
		void operator()(T... params) const;

		int add_callback(const Callback& callback);

		int operator+=(const Callback& callback);

		void remove_callback(const int callback_handle);

		void operator-=(const int callback_handle);
	};

	
}

#include "Event.tpp"