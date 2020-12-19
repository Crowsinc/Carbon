#include "Identified.hpp"

namespace cbn
{
	
	//-------------------------------------------------------------------------------------

	// Start at 1 because the UID 0 is the default empty UID
	uint64_t Identified::s_NextUID = 1;

	//-------------------------------------------------------------------------------------

	Identified::Identified()
		: m_UID(s_NextUID++) {}
	
	//-------------------------------------------------------------------------------------

	Identified::Identified(const Identified& other)
		: m_UID(s_NextUID++) {}

	//-------------------------------------------------------------------------------------

	Identified::Identified(Identified&& other)
		: m_UID(other.m_UID)
	{
		// Make the previous instance invalid
		other.m_UID = 0;
	}

	//-------------------------------------------------------------------------------------

	uint64_t Identified::uid() const
	{
		return m_UID;
	}

	//-------------------------------------------------------------------------------------

}