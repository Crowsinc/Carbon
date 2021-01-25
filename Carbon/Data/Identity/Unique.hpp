#pragma once

#include <cstdint>

namespace cbn
{

	struct GlobalScope;

	template<typename Scope = GlobalScope>
	class Unique
	{
	private:

		static uint64_t s_NextUID;

		uint64_t m_UID;

	public:

		Unique();

		Unique(const Unique& other);

		Unique(Unique&& other);

		uint64_t uid() const;

	};

	//-------------------------------------------------------------------------------------

	template<typename Scope>
	uint64_t Unique<Scope>::s_NextUID = 1;

	//-------------------------------------------------------------------------------------

	template<typename Scope>
	inline Unique<Scope>::Unique()
		: m_UID(s_NextUID++)
	{}
	
	//-------------------------------------------------------------------------------------

	template<typename Scope>
	inline Unique<Scope>::Unique(const Unique& other)
		: m_UID(s_NextUID++)
	{}

	//-------------------------------------------------------------------------------------

	template<typename Scope>
	inline Unique<Scope>::Unique(Unique&& other)
		: m_UID(other.m_UID)
	{}

	//-------------------------------------------------------------------------------------

	template<typename Scope>
	inline uint64_t Unique<Scope>::uid() const
	{
		return m_UID;
	}

	//-------------------------------------------------------------------------------------
}

