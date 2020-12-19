#pragma once

#include "Identifier.hpp"

namespace cbn
{

	class Identifiable
	{
	private:

		Identifier m_Identity;

	public:

		Identifiable() = default;
		
		Identifiable(const Identifier& identifier);

		void set_identity(const Identifier& identifier);

		void set_alias(const String& alias);

		const Identifier& identifier() const;

		bool shares_identity(const Identifiable& other) const;
	};

}