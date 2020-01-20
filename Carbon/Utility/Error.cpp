#include "Error.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	Error::Error(const uint32_t error_code, const std::string& error_string)
	 : m_ErrorCode(error_code),
	   m_ErrorString(error_string) {}

	//-------------------------------------------------------------------------------------

	const std::string& Error::to_string() const
	{
		return m_ErrorString;
	}

	//-------------------------------------------------------------------------------------

	uint32_t Error::get_code() const
	{
		return m_ErrorCode;
	}
	
	//-------------------------------------------------------------------------------------

	Error::operator const uint32_t() const
	{
		return m_ErrorCode;
	}

	//-------------------------------------------------------------------------------------

	std::ostream& operator<<(std::ostream& stream, const Error& error)
	{
		stream << error.to_string();
	}

	//-------------------------------------------------------------------------------------

}