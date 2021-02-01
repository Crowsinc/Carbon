#include "Buffer.hpp"

#include "../../Utility/Enum.hpp"
#include "../../Diagnostics/Assert.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	// Initialize bound buffer state with all buffer types unbound. We do this via
	// a zero-initialization of a raw array which is then converted to an std array.
	std::array<GLuint, 3> Buffer::s_BoundBuffers = std::to_array<GLuint, 3>({0});

	//-------------------------------------------------------------------------------------

	GLenum Buffer::to_opengl_target(const BufferTarget& target)
	{
		switch(target)
		{
			case BufferTarget::ELEMENT_BUFFER: return GL_ELEMENT_ARRAY_BUFFER;
			case BufferTarget::UNIFORM_BUFFER: return GL_UNIFORM_BUFFER;
			case BufferTarget::VERTEX_BUFFER: return GL_ARRAY_BUFFER;
		
			default: CBN_Assert(false, "Undefined buffer target");
		}
		return 0;
	}
	
	//-------------------------------------------------------------------------------------

	Buffer::Buffer(const BufferTarget target)
		: m_Target(target)
	{
		glGenBuffers(1, &m_BufferID);
	}

	//-------------------------------------------------------------------------------------

	Buffer::~Buffer()
	{
		// Before we destroy the object, we need to ensure the static bounded object tracker
		// doesnt consider it as being bound. Otherwise issues will arise when a new object
		// takes its ID.
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
			s_BoundBuffers[value(m_Target)] = 0;
			glBindBuffer(to_opengl_target(m_Target), 0);
		}
	}

	//-------------------------------------------------------------------------------------

	void Buffer::force_bind()
	{
		s_BoundBuffers[value(m_Target)] = m_BufferID;
		glBindBuffer(to_opengl_target(m_Target), m_BufferID);
	}

	//-------------------------------------------------------------------------------------

	bool Buffer::is_bound() const
	{
		return s_BoundBuffers[value(m_Target)] == m_BufferID;
	}

	//-------------------------------------------------------------------------------------

	BufferTarget Buffer::get_target() const
	{
		return m_Target;
	}

	//-------------------------------------------------------------------------------------

}