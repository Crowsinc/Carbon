#pragma once

#include <string>

namespace cbn
{

	class Error
	{
	private:

		const uint32_t m_ErrorCode;
		const std::string m_ErrorString;

	public:

		Error(const uint32_t error_code, const std::string& error_string = "");
	
		const std::string& to_string() const;

		uint32_t get_code() const;

		operator const uint32_t() const;

	};

	std::ostream& operator<<(std::ostream& stream, const Error& error);
}