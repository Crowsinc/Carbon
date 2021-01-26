#include "Timer.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	Timer::Timer() {}

	//-------------------------------------------------------------------------------------

	void Timer::start(const Time& interval)
	{
		m_Interval = interval;
		m_Stopwatch.start();
	}

	//-------------------------------------------------------------------------------------

	void Timer::stop()
	{
		m_Stopwatch.stop();
	}

	//-------------------------------------------------------------------------------------

	void Timer::update()
	{
		if(m_Stopwatch.elapsed() >= m_Interval)
		{
			m_Stopwatch.restart();
			TimerEvent.dispatch();
		}
	}

	//-------------------------------------------------------------------------------------

	Time Timer::elapsed() const
	{
		return m_Stopwatch.elapsed();
	}

	//-------------------------------------------------------------------------------------

	const Time& Timer::interval() const
	{
		return m_Interval;
	}
	
	//-------------------------------------------------------------------------------------

	bool Timer::is_running() const
	{
		return m_Stopwatch.is_running();
	}

	//-------------------------------------------------------------------------------------

	AutoTimer::AutoTimer()
		: m_Running(false)
	{}

	//-------------------------------------------------------------------------------------

	AutoTimer::~AutoTimer()
	{
		stop();
	}

	//-------------------------------------------------------------------------------------

	void AutoTimer::start(const Time& interval)
	{
		m_Running = true;
		m_Interval = interval;

		m_TimerThread = std::thread([&, this, sleep_time_ns = interval.nanoseconds()]()
		{
			using TimeFormat = std::chrono::nanoseconds;
			const TimeFormat sleep_time(static_cast<TimeFormat::rep>(sleep_time_ns));

			while(m_Running)
			{
				std::this_thread::sleep_for(sleep_time);
				TimerEvent.dispatch();
			}
		});
	}
	
	//-------------------------------------------------------------------------------------

	void AutoTimer::stop()
	{
		m_Running = false;
		m_TimerThread.join();
	}
	
	//-------------------------------------------------------------------------------------

	bool AutoTimer::is_running() const
	{
		return m_Running;
	}
	
	//-------------------------------------------------------------------------------------

	const Time& AutoTimer::interval() const
	{
		return m_Interval;
	}
	
	//-------------------------------------------------------------------------------------

}