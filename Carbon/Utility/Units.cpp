#include "Units.hpp"

namespace cbn::units
{
	//-------------------------------------------------------------------------------------

	TimeUnit::TimeUnit(const double ratio, const std::string& prefix)
		: m_Ratio(ratio),
		 m_Prefix(prefix) {}
	
	//-------------------------------------------------------------------------------------

	const std::string& TimeUnit::get_prefix() const
	{
		return m_Prefix;
	}
	
	//-------------------------------------------------------------------------------------

	double TimeUnit::get_seconds_ratio() const
	{
		return m_Ratio;
	}

	//-------------------------------------------------------------------------------------

	std::ostream& operator<<(std::ostream& stream, const TimeUnit& time_unit)
	{
		return stream << time_unit.get_prefix();
	}
	
	//-------------------------------------------------------------------------------------

	double convert(const double value, const TimeUnit& from, const TimeUnit& to)
	{
		return value * from.get_seconds_ratio() / to.get_seconds_ratio();
	}

	//-------------------------------------------------------------------------------------

}

