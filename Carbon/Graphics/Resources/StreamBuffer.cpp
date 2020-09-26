#include "StreamBuffer.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	SRes<StreamBuffer> StreamBuffer::Allocate(const Buffer::Target target, const uint64_t byte_size, const bool unsynchronized, const Version opengl_version)
	{
		// Create the stream buffer now then allocate memory for it afterwards. 
		auto stream_buffer = SRes<StreamBuffer>(new StreamBuffer(target, byte_size, unsynchronized));

		// If OpenGL 4.4 is supported, create immutable storage otherwise use the normal buffer data.
		if(opengl_version >= Version{4, 4})
		{
			glBufferStorage(static_cast<GLenum>(target), byte_size, NULL, GL_MAP_WRITE_BIT);
		}
		else
		{
			glBufferData(static_cast<GLenum>(target), byte_size, NULL, GL_STREAM_DRAW);
		}

		return stream_buffer;
	}

	//-------------------------------------------------------------------------------------

	StreamBuffer::StreamBuffer(const Buffer::Target target, const uint64_t byte_size, const bool unsynchronized)
		: Buffer(target),
		m_Mapped(false),
		m_ByteSize(byte_size),
		m_Unsynchronized(unsynchronized),
		m_MappingFlags(GL_MAP_WRITE_BIT | (unsynchronized ? GL_MAP_UNSYNCHRONIZED_BIT : NULL)) {}
	
	//-------------------------------------------------------------------------------------

	void* StreamBuffer::map()
	{
		CBN_Assert(!m_Mapped, "Cannot map a buffer which is already mapped");

		bind();
		m_Mapped = true;
		return glMapBufferRange(static_cast<GLenum>(get_target()), 0, m_ByteSize, m_MappingFlags);
	}
	
	//-------------------------------------------------------------------------------------

	void StreamBuffer::unmap()
	{
		CBN_Assert(m_Mapped, "Cannot unmap buffer which is not already mapped");
		
		bind();
		m_Mapped = false;
		glUnmapBuffer(static_cast<GLenum>(get_target()));
	}
	
	//-------------------------------------------------------------------------------------

	uint64_t StreamBuffer::size() const
	{
		return m_ByteSize;
	}

	//-------------------------------------------------------------------------------------
	
	bool StreamBuffer::is_mapped() const
	{
		return m_Mapped;
	}
	
	//-------------------------------------------------------------------------------------

	bool StreamBuffer::is_unsynchronized() const
	{
		return m_Unsynchronized;
	}

	//-------------------------------------------------------------------------------------

}