#pragma once

#include <string>
#include <ostream>

namespace cbn
{

	class Unit
	{
	public:
		
		static double convert(const double value, const Unit& from_unit, const Unit& to_unit);
	
	private:
	
		const double m_Ratio;
		const std::string m_Prefix;
		
	public:
		
		Unit(const double ratio, const std::string& prefix);

		double get_base_unit_ratio() const;
		
		const std::string& get_prefix() const;

		double convert_to(const double value, const Unit& to_unit) const;

		friend std::ostream& operator<<(std::ostream& stream, const Unit& unit);
	};

	
	// Time units
	using TimeUnit = Unit;
	const TimeUnit Days(86400.0, "d");
	const TimeUnit Hours(3600.0, "h");
	const TimeUnit Minutes(60.0, "m");
	const TimeUnit Seconds(1.0, "s");
	const TimeUnit Milliseconds(0.001, "ms");
	const TimeUnit Microseconds(0.000001, "us");
	const TimeUnit Nanoseconds(0.000000001, "ns");
	
	// Storage units
	using StorageUnit = Unit;
	const StorageUnit Terabytes(pow(2,40),"TB");
	const StorageUnit Gigabytes(pow(2, 30), "GB");
	const StorageUnit Megabytes(pow(2, 20), "MB");
	const StorageUnit Kilobytes(pow(2,10),"KB");
	const StorageUnit Bytes (1.0, "B");
	const StorageUnit Bits(1.0/8.0,"b");

}

