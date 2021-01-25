#pragma once

#include <functional>

#include "../../Data/Identity/Unique.hpp"

namespace cbn
{

	// Simply a way to store a callback in an identifiable way. 
	// TODO: improve and generalise later. 
	template<typename ...Args>
	class EventCallback : public Unique<EventCallback<Args...>>
	{
	public:
		using RawType = std::function<void(Args...)>;
	private:
		const RawType m_Function;
	public:

		EventCallback(const RawType& function);

		void invoke(Args... args) const;
	};

	//-------------------------------------------------------------------------------------

	template<typename ...Args>
	inline EventCallback<Args...>::EventCallback(const RawType& function)
		: m_Function(function) {}

	//-------------------------------------------------------------------------------------

	template<typename ...Args>
	inline void EventCallback<Args...>::invoke(Args... args) const
	{
		m_Function(args...);
	}

	//-------------------------------------------------------------------------------------

}