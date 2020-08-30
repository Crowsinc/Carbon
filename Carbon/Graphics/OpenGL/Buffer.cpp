#include "Buffer.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	std::unordered_map<Buffer::Target, GLint> m_BoundBuffers;

	//-------------------------------------------------------------------------------------

	Buffer::Buffer(const Target target)
		: m_Target(target)
	{
		glGenBuffers(1, &m_BufferID);
	}

	//-------------------------------------------------------------------------------------

	Buffer::~Buffer()
	{
		unbind();
		glDeleteBuffers(1, &m_BufferID);
	}

	//-------------------------------------------------------------------------------------

	void Buffer::bind()
	{
		if(!is_bound())
		{
			m_BoundBuffers[m_Target] = m_BufferID;
			glBindBuffer(static_cast<GLenum>(m_Target), m_BufferID);
		}
	}

	//-------------------------------------------------------------------------------------

	void Buffer::unbind()
	{
		if(is_bound())
		{
			m_BoundBuffers[m_Target] = 0;
			glBindBuffer(static_cast<GLenum>(m_Target), 0);
		}
	}

	//-------------------------------------------------------------------------------------

	bool Buffer::is_bound() const
	{
		return m_BoundBuffers[m_Target] == m_BufferID;
	}

	//-------------------------------------------------------------------------------------

	Buffer::Target Buffer::get_target() const
	{
		return m_Target;
	}

	//-------------------------------------------------------------------------------------

}