#pragma once

#include "../Events/EventHost.hpp"
#include "Stopwatch.hpp"

namespace cbn
{

	class Ticker
	{
	public:

		EventHost<Ticker, uint64_t, float> TickEvent;

	private:

		Stopwatch m_Stopwatch;

	public:

		Ticker();

		void tick() const;

		float delta_time() const; // time between curr tick and last tick in seconds. or curr time and last tick time? 

	};


}