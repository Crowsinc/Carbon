#pragma once

#include <string>
#include <chrono>

namespace cbn
{
	//TODO: make it easier to specify a specific time in any format
	class Time
	{
	public:

		using TimePoint = std::chrono::high_resolution_clock::time_point;

		static Time Now();

		static std::string Timestamp(const char* format = "%F %T");

		static Time Hours(const double amount);
		
		static Time Minutes(const double amount);
		
		static Time Seconds(const double amount);
		
		static Time Milliseconds(const double amount);
		
		static Time Microseconds(const double amount);
		
		static Time Nanoseconds(const uint64_t amount);

	private:

		std::chrono::nanoseconds m_Time;

	public:

		Time();

		Time(const TimePoint& time);

		Time(const uint64_t nanoseconds);

		Time(const std::chrono::nanoseconds nanoseconds);

		Time(const Time& other);

		double hours() const;

		double minutes() const;

		double seconds() const;
		
		double milliseconds() const;
		
		double microseconds() const;
		
		double nanoseconds() const;
	
		void operator=(const Time& other);

		void operator+=(const Time& other);

		void operator-=(const Time& other);
		
		Time operator+(const Time& other) const;

		Time operator-(const Time& other) const;

		bool operator==(const Time& other) const;

		bool operator!=(const Time& other) const;

		bool operator>(const Time& other) const;
		
		bool operator>=(const Time& other) const;
	
		bool operator<(const Time& other) const;
		
		bool operator<=(const Time& other) const;

	};

}

