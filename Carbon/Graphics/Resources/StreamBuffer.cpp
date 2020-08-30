#include "StreamBuffer.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	SRes<StreamBuffer> StreamBuffer::Allocate(const Buffer::Target target, const uint64_t byte_size, const bool unsynchronized, const Version opengl_version)
	{
		// If we are using OpenGL 4.4+, then use persistently mapped buffers.
		const bool GL44 = opengl_version >= Version{4, 4};

		// Create the stream buffer now then allocate memory for it afterwards. 
		auto stream_buffer = SRes<StreamBuffer>(new StreamBuffer(target, byte_size, unsynchronized, GL44));

		// If OpenGL 4.4 is supported, create immutable storage otherwise use the normal buffer data.
		if(GL44)
		{
			GLbitfield creation_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
			glBufferStorage(static_cast<GLenum>(target), byte_size, NULL, creation_flags);
		}
		else
		{
			glBufferData(static_cast<GLenum>(target), byte_size, NULL, GL_STREAM_DRAW);
		}

		return stream_buffer;
	}

	//-------------------------------------------------------------------------------------

	StreamBuffer::StreamBuffer(const Buffer::Target target, const uint64_t byte_size, const bool unsynchronized, const bool persistent)
		: Buffer(target),
		m_IsMapped(false),
		m_MappedPtr(nullptr),
		m_ByteSize(byte_size),
		m_Persistent(persistent),
		m_Unsynchronized(unsynchronized)
	{
		m_MappingFlags = GL_MAP_WRITE_BIT;
		if(unsynchronized) m_MappingFlags |= GL_MAP_UNSYNCHRONIZED_BIT;
	}
	
	//-------------------------------------------------------------------------------------

	void* StreamBuffer::map(const uint64_t byte_offset, const uint64_t length)
	{
		CBN_Assert(length + byte_offset < m_ByteSize, "Attempting to map outside of buffer");
		CBN_Assert(!m_IsMapped, "Cannot map a buffer which is already mapped");

		m_MappedOffset = byte_offset;
		if(length == 0)
			m_MappedLength = m_ByteSize - byte_offset;
		else
			m_MappedLength = length;

		if(m_Persistent)
		{
			// If its persistently mapped, but it hasnt been mapped yet then we need to map the buffer for the first time
			if(m_MappedPtr == nullptr)
			{
				bind();
				m_MappedPtr = glMapBufferRange(static_cast<GLenum>(get_target()), 0, m_ByteSize, m_MappingFlags | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
			}

			return static_cast<uint8_t*>(m_MappedPtr) + byte_offset;
		} 
		else
		{
			// If we are here, its not persistently mapped so we need to bind the buffer then map it
			bind();
			return glMapBufferRange(static_cast<GLenum>(get_target()), m_MappedOffset, m_MappedLength, m_MappingFlags);
		}
	}
	
	//-------------------------------------------------------------------------------------

	void StreamBuffer::unmap()
	{
		CBN_Assert(m_IsMapped, "Cannot unmap buffer which is not already mapped");

		m_IsMapped = false;

		// If we are using persistently mapped buffers, then flush the mapped range
		if(m_Persistent)
		{
			glFlushMappedBufferRange(GL_ARRAY_BUFFER, m_MappedOffset, m_MappedLength); 
			return;
		}
		else
		{
			glUnmapBuffer(static_cast<GLenum>(get_target()));
		}
	}
	
	//-------------------------------------------------------------------------------------

	uint64_t StreamBuffer::size() const
	{
		return m_ByteSize;
	}

	//-------------------------------------------------------------------------------------
	
	bool StreamBuffer::is_mapped() const
	{
		return m_IsMapped;
	}
	
	//-------------------------------------------------------------------------------------

	bool StreamBuffer::is_persistent() const
	{
		return m_Persistent;
	}
	
	//-------------------------------------------------------------------------------------

	bool StreamBuffer::is_unsynchronized() const
	{
		return m_Unsynchronized;
	}

	//-------------------------------------------------------------------------------------

}