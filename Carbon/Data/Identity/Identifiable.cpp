#include "Identifiable.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	Identifiable::Identifiable(const Identifier& identifier)
	{
		set_identity(identifier);
	}

	//-------------------------------------------------------------------------------------

	void Identifiable::set_identity(const Identifier& identifier)
	{
		m_Identity = identifier;
	}
	
	//-------------------------------------------------------------------------------------

	void Identifiable::set_alias(const String& alias)
	{
		m_Identity = {alias};
	}
	
	//-------------------------------------------------------------------------------------

	const Identifier& Identifiable::identifier() const
	{
		return m_Identity;
	}
	
	//-------------------------------------------------------------------------------------

	bool Identifiable::shares_identity(const Identifiable& other) const
	{
		return m_Identity == other.m_Identity;
	}
	
	//-------------------------------------------------------------------------------------

}