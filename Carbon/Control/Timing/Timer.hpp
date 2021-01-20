#pragma once

#include <thread>

#include "Stopwatch.hpp"
#include "../Events/EventHost.hpp"
#include "../../Utility/Units.hpp"

namespace cbn
{
	class Timer
	{
	public:

		EventHost<Timer> TimerEvent;

	private:

		Stopwatch m_Stopwatch;

	public:

		Timer(const double duration, const TimeUnit& unit = cbn::Milliseconds);

		void configure(const double duration, const TimeUnit& unit = cbn::Milliseconds);

		void restart();

		void update();

	};

	// This is a timer which does automatic updating
	class AutoTimer
	{
	public:

		EventHost<AutoTimer> TimerEvent;

	private:

		Stopwatch m_Stopwatch;
		std::thread m_TimerThread;

	public:

		AutoTimer(const double duration, const TimeUnit& unit = cbn::Milliseconds);

		void configure(const double duration, const TimeUnit& unit = cbn::Milliseconds);

		void restart();

		void stop();

	};

}