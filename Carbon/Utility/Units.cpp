#include "Units.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	double Unit::convert(const double value, const Unit& from, const Unit& to)
	{
		return value * from.get_base_unit_ratio() / to.get_base_unit_ratio();
	}

	//-------------------------------------------------------------------------------------

	Unit::Unit(const double ratio, const std::string& prefix)
		: m_Ratio(ratio),
		 m_Prefix(prefix) {}
	
	//-------------------------------------------------------------------------------------

	double Unit::convert_to(const double value, const Unit& to_unit) const
	{
		return convert(value, *this, to_unit);
	}
	
	//-------------------------------------------------------------------------------------

	const std::string& Unit::get_prefix() const
	{
		return m_Prefix;
	}
	
	//-------------------------------------------------------------------------------------

	double Unit::get_base_unit_ratio() const
	{
		return m_Ratio;
	}

	//-------------------------------------------------------------------------------------

	std::ostream& operator<<(std::ostream& stream, const Unit& unit)
	{
		return stream << unit.get_prefix();
	}

	//-------------------------------------------------------------------------------------

}

