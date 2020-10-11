#include "Buffer.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	std::unordered_map<BufferTarget, GLint> Buffer::s_BoundBuffers;

	//-------------------------------------------------------------------------------------

	Buffer::Buffer(const BufferTarget target)
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
			force_bind();
	}

	//-------------------------------------------------------------------------------------

	void Buffer::unbind()
	{
		if(is_bound())
		{
			s_BoundBuffers[m_Target] = 0;
			glBindBuffer(static_cast<GLenum>(m_Target), 0);
		}
	}

	//-------------------------------------------------------------------------------------

	void Buffer::force_bind()
	{
		s_BoundBuffers[m_Target] = m_BufferID;
		glBindBuffer(static_cast<GLenum>(m_Target), m_BufferID);
	}

	//-------------------------------------------------------------------------------------

	bool Buffer::is_bound() const
	{
		return s_BoundBuffers[m_Target] == m_BufferID;
	}

	//-------------------------------------------------------------------------------------

	BufferTarget Buffer::get_target() const
	{
		return m_Target;
	}

	//-------------------------------------------------------------------------------------

}