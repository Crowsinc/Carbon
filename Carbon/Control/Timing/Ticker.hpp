#pragma once

#include "../Events/EventHost.hpp"
#include "Stopwatch.hpp"

namespace cbn
{

	class Ticker
	{
	public:

		EventHost<Ticker, uint64_t, Time> TickEvent;

	private:

		Stopwatch m_Stopwatch;
		uint64_t m_Counter;
		Time m_DeltaTime;

	public:

		Ticker();

		void tick();

		Time delta_time() const; 

		Time elapsed_time() const;

		uint64_t tick_count() const;

		void reset_counter();

	};


}