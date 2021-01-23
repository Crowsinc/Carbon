#pragma once

#include <thread>

#include "Time.hpp"
#include "Stopwatch.hpp"
#include "../Events/EventHost.hpp"

namespace cbn
{
	class Timer
	{
	public:

		EventHost<Timer> TimerEvent;

	private:

		Stopwatch m_Stopwatch;
		Time m_Interval;

	public:

		Timer();

		void start(const Time& interval);

		void stop();

		void update();

		Time elapsed() const;

		const Time& interval() const;

		bool is_running() const;
	};

	// This is a timer which does automatic updating
	class AutoTimer
	{
	public:

		EventHost<AutoTimer> TimerEvent;

	private:

		bool m_Running;
		Time m_Interval;
		Stopwatch m_Stopwatch;
		std::thread m_TimerThread;

	public:

		AutoTimer();

		~AutoTimer();

		void start(const Time& interval);

		void stop();

		bool is_running() const;

		const Time& interval() const;
	};

}