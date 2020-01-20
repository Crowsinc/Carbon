#pragma once

namespace cbn
{

	class Version
	{
	private:

		const unsigned m_Major, m_Minor, m_Build;

	public:

		Version(unsigned major, unsigned minor = 0, unsigned build = 0);

		bool is_newer_than(const Version& version) const;
		
		bool is_newer_than(unsigned major, unsigned minor = 0, unsigned build = 0) const;

		bool is_older_than(const Version& version) const;
				
		bool is_older_than(unsigned major, unsigned minor = 0, unsigned build = 0) const;

		bool matches(const Version& version) const;
		
		bool matches(unsigned major, unsigned minor = 0, unsigned build = 0) const;
		
		unsigned get_major_version() const;
		
		unsigned get_minor_version() const;
		
		unsigned get_build_number() const;

		bool operator=(const Version& version);
		
		bool operator>(const Version& version) const;
		
		bool operator<(const Version& version) const;
		
		bool operator>=(const Version& version) const;
		
		bool operator<=(const Version& version) const;
		
		bool operator==(const Version& version) const;
		
		bool operator!=(const Version& version) const;

	};


}