#include "Version.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	Version::Version(const unsigned major, const unsigned minor, const unsigned build)
		: m_Major(major), m_Minor(minor), m_Build(build)
	{}
	
	//-------------------------------------------------------------------------------------
	
	bool Version::is_newer_than(const Version & version) const
	{
		return m_Major > version.m_Major || (m_Major == version.m_Major && m_Minor > version.m_Minor) || (m_Major == version.m_Major && m_Minor == version.m_Minor && m_Build > version.m_Build);
	}
	
	//-------------------------------------------------------------------------------------

	bool Version::is_newer_than(const unsigned major, const unsigned minor, const unsigned build) const
	{
		return m_Major > major || (m_Major == major && m_Minor > minor) || (m_Major == major && m_Minor == minor && m_Build > build);
	}

	//-------------------------------------------------------------------------------------

	bool Version::is_older_than(const Version& version) const
	{
		return m_Major < version.m_Major || (m_Major == version.m_Major && m_Minor < version.m_Minor) || (m_Major == version.m_Major && m_Minor == version.m_Minor && m_Build < version.m_Build);
	}

	//-------------------------------------------------------------------------------------

	bool Version::is_older_than(const unsigned major, const unsigned minor, const unsigned build) const
	{
		return m_Major < major || (m_Major == major && m_Minor < minor) || (m_Major == major && m_Minor == minor && m_Build < build);
	}

	//-------------------------------------------------------------------------------------

	bool Version::matches(const Version& version) const
	{
		return m_Major == version.m_Major && m_Minor == version.m_Minor && m_Build == version.m_Build;
	}

	//-------------------------------------------------------------------------------------

	bool Version::matches(const unsigned major, const unsigned minor, const unsigned build) const
	{
		return m_Major == major && m_Minor == minor && m_Build == build;
	}

	//-------------------------------------------------------------------------------------

	unsigned Version::get_major_version() const
	{
		return m_Major;
	}

	//-------------------------------------------------------------------------------------

	unsigned Version::get_minor_version() const
	{
		return m_Minor;
	}

	//-------------------------------------------------------------------------------------

	unsigned Version::get_build_number() const
	{
		return m_Build;
	}

	//-------------------------------------------------------------------------------------

	bool Version::operator>(const Version& version) const
	{
		return is_newer_than(version);
	}

	//-------------------------------------------------------------------------------------

	bool Version::operator<(const Version& version) const
	{
		return is_older_than(version);
	}

	//-------------------------------------------------------------------------------------

	bool Version::operator==(const Version& version) const
	{
		return matches(version);
	}

	//-------------------------------------------------------------------------------------

	bool Version::operator!=(const Version& version) const
	{
		return !matches(version);
	}

	//-------------------------------------------------------------------------------------
}