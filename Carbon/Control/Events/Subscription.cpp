#include "Subscription.hpp"

#include "../../Diagnostics/Assert.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	Subscription::Subscription(std::function<void()> unsubscriber)
		: m_UnsubscribeFunction(unsubscriber),
		m_Subscribed(true)
	{}

	//-------------------------------------------------------------------------------------

	Subscription::Subscription()
		: m_Subscribed(false)
	{}

	//-------------------------------------------------------------------------------------

	Subscription::~Subscription()
	{
		if(is_subscribed())
			unsubscribe();
	}

	//-------------------------------------------------------------------------------------

	Subscription::Subscription(Subscription&& other)
		: m_UnsubscribeFunction(std::move(other.m_UnsubscribeFunction)),
		m_Subscribed(other.m_Subscribed)
	{
		// Flag it as not subscribed just to ensure it never tries to unsubscribe
		other.m_Subscribed = false;
	}

	//-------------------------------------------------------------------------------------

	void Subscription::operator=(Subscription&& other)
	{
		m_UnsubscribeFunction = std::move(other.m_UnsubscribeFunction);
		m_Subscribed = other.m_Subscribed;

		// Flag it as not subscribed just to ensure it never tries to unsubscribe
		other.m_Subscribed = false;
	}

	//-------------------------------------------------------------------------------------

	bool Subscription::is_subscribed() const
	{
		return m_Subscribed;
	}

	//-------------------------------------------------------------------------------------

	void Subscription::unsubscribe()
	{
		CBN_Assert(m_Subscribed, "Cannot unsubscribe an already unsubscribed subscription");

		m_UnsubscribeFunction();
		m_Subscribed = false;
	}

	//-------------------------------------------------------------------------------------

}