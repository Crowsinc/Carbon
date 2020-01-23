#include "Stopwatch.hpp"

#include "Assert.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	Stopwatch::Stopwatch() 
		: m_StartTime(std::chrono::high_resolution_clock::now()),
		  m_StopTime(m_StartTime),
		  m_Started(false)
	{}
	
	//-------------------------------------------------------------------------------------

	void Stopwatch::start()
	{
		m_StartTime = std::chrono::high_resolution_clock::now();
		m_Started = true;
	}
	
	//-------------------------------------------------------------------------------------

	double Stopwatch::stop(const cbn::TimeUnit & time_unit)
	{
		CBN_Assert(m_Started, "Stopwatch cannot be stopped without first being started.");

		m_StopTime = std::chrono::high_resolution_clock::now();
		m_Started = false;

		return get_elapsed_time(time_unit);
	}
	
	//-------------------------------------------------------------------------------------

	double Stopwatch::restart(const cbn::TimeUnit& time_unit)
	{
		const auto elapsed = stop();
		start();

		return elapsed;
	}
	
	//-------------------------------------------------------------------------------------

	double Stopwatch::get_elapsed_time(const cbn::TimeUnit& time_unit) const
	{
		// If the stopwatch has been started but not stopped, then return the time between
		// starting and now, otherwise return the time between the start and stopping events. 
		const auto end_time = (m_Started) ? std::chrono::high_resolution_clock::now() : m_StopTime;
		const auto difference_ns = static_cast<double>((end_time - m_StartTime).count());

		return cbn::Unit::convert(difference_ns, cbn::Nanoseconds, time_unit);
	}

	//-------------------------------------------------------------------------------------

}