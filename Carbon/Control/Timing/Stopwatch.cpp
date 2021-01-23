#include "Stopwatch.hpp"

#include "../../Diagnostics/Assert.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	Stopwatch::Stopwatch()
		: m_Running(false),
		m_StartTime(),
		m_Elapsed()
	{}
	
	//-------------------------------------------------------------------------------------

	void Stopwatch::start()
	{
		m_Running = true;
		m_StartTime = Time::Now();
	}

	//-------------------------------------------------------------------------------------

	void Stopwatch::stop()
	{
		CBN_Assert(is_running(), "Cannot stop a Stopwatch which is not running");
		
		m_Elapsed = Time::Now() - m_StartTime;
		m_Running = false;
	}

	//-------------------------------------------------------------------------------------

	Time Stopwatch::restart()
	{
		// Collect the current elapsed time, then simply start the stopwatch again. 
		// Note that we don't need to stop it in order to start it again. 
		const auto elapsed = this->elapsed();
		start();
		return elapsed;
	}

	//-------------------------------------------------------------------------------------

	Time Stopwatch::elapsed() const
	{
		// If the stopwatch is running, then return the time elapsed since it was started.
		// Otherwise we just return the time elapsed between when it started and stopped. 
		return is_running() ? (Time::Now() - m_StartTime) : m_Elapsed;
	}

	//-------------------------------------------------------------------------------------

	bool Stopwatch::is_running() const
	{
		return m_Running;
	}

	//-------------------------------------------------------------------------------------

}