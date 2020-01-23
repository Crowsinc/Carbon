#pragma once

#include <chrono>

#include "../Utility/Units.hpp"

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

		double stop(const cbn::TimeUnit& time_unit = cbn::Milliseconds);
		
		double restart(const cbn::TimeUnit& time_unit = cbn::Milliseconds);

		double get_elapsed_time(const cbn::TimeUnit& time_unit = cbn::Milliseconds) const;

	};

}

