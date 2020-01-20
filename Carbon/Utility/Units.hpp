#pragma once

#include <string>
#include <ostream>

namespace cbn::units
{

	class TimeUnit
	{
	private:
		const double m_Ratio;
		const std::string m_Prefix;
		
	public:
		TimeUnit(const double ratio, const std::string& prefix);

		const std::string& get_prefix() const;

		double get_seconds_ratio() const;

	};

	const TimeUnit Days(86400.0, "s");
	const TimeUnit Hours(3600.0, "s");
	const TimeUnit Minutes(60.0, "s");
	const TimeUnit Seconds(1.0, "s");
	const TimeUnit Milliseconds(0.001, "s");
	const TimeUnit Microseconds(0.000001, "s");
	const TimeUnit Nanoseconds(0.000000001, "s");
	
	std::ostream& operator<<(std::ostream& stream, const TimeUnit& time_unit);

	double convert(const double value, const TimeUnit& from, const TimeUnit& to);


}

