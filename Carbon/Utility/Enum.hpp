#pragma once

namespace cbn
{
	template<typename E>
	class Enum
	{
	private:

		E m_Value;

	public:

		Enum() = default;

		Enum(const E value) : m_Value(value) {}

		int to_int() const
		{
			return static_cast<int>(m_Value);
		}

		void operator=(const E new_value)
		{
			m_Value = new_value;
		}

		void operator=(const Enum<E> new_value)
		{
			m_Value = new_value.m_Value;
		}

		bool operator==(const E other) const
		{
			return m_Value == other;
		}

		bool operator==(const Enum<E> other) const
		{
			return m_Value == other.m_Value;
		}

		operator const E() const
		{
			return m_Value;
		}

	};

}