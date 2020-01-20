#pragma once

#include <chrono>

#include "Units.hpp"

namespace cbn
{

	class Stopwatch
	{
	private:

		std::chrono::high_resolution_clock::time_point m_StartTime;
		std::chrono::high_resolution_clock::time_point m_StopTime;
		bool m_Started;

	public:

		Stopwatch();

		void start();

		double stop(const cbn::units::TimeUnit& time_unit = cbn::units::Milliseconds);
		
		double restart(const cbn::units::TimeUnit& time_unit = cbn::units::Milliseconds);

		double get_elapsed_time(const cbn::units::TimeUnit& time_unit = cbn::units::Milliseconds) const;

	};

}

