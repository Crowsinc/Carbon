#pragma once

#include "Time.hpp"

namespace cbn
{

	class Stopwatch
	{
	private:

		Time m_StartTime, m_Elapsed;
		bool m_Running;

	public:

		Stopwatch();

		void start();

		void stop();

		Time restart();

		Time elapsed() const;

		bool is_running() const;

	};

}

