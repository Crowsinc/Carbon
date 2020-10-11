#include "StreamBuffer.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	SRes<StreamBuffer> StreamBuffer::Allocate(const BufferTarget target, const uint64_t byte_size, const bool synchronized, const Version opengl_version)
	{
		//NOTE: we are not currently using the openGL version for optimisations, but we may in the future
		
		// Create the stream buffer now then allocate memory for it afterwards. 
		auto stream_buffer = SRes<StreamBuffer>(new StreamBuffer(target, byte_size, synchronized));

		stream_buffer->bind();
		glBufferData(static_cast<GLenum>(target), byte_size, NULL, GL_STREAM_DRAW);
		stream_buffer->unbind();

		return stream_buffer;
	}

	//-------------------------------------------------------------------------------------

	StreamBuffer::StreamBuffer(const BufferTarget target, const uint64_t byte_size, const bool synchronized)
		: Buffer(target),
		m_Mapped(false),
		m_ByteSize(byte_size),
		m_Synchronized(synchronized),
		m_MappingFlags(GL_MAP_WRITE_BIT | (synchronized ? NULL : GL_MAP_UNSYNCHRONIZED_BIT)) {}
	
	//-------------------------------------------------------------------------------------

	void* StreamBuffer::map()
	{
		CBN_Assert(!is_mapped(), "Cannot map a buffer which is already mapped");

		bind();
		m_Mapped = true;
		return glMapBufferRange(static_cast<GLenum>(get_target()), 0, m_ByteSize, m_MappingFlags);
	}
	
	//-------------------------------------------------------------------------------------

	void* StreamBuffer::map(const uint64_t offset, const uint64_t length)
	{
		CBN_Assert(!m_Mapped, "Cannot map a buffer which is already mapped");
		CBN_Assert(offset + length < m_ByteSize, "Cannot map a buffer which is already mapped");

		bind();
		m_Mapped = true;
		return glMapBufferRange(static_cast<GLenum>(get_target()), offset, length, m_MappingFlags);
	}

	//-------------------------------------------------------------------------------------

	void StreamBuffer::unmap()
	{
		CBN_Assert(is_mapped(), "Cannot unmap buffer which is not already mapped");
		
		bind();
		m_Mapped = false;
		glUnmapBuffer(static_cast<GLenum>(get_target()));
	}

	//-------------------------------------------------------------------------------------

	void StreamBuffer::reallocate()
	{
		reallocate(size());
	}

	//-------------------------------------------------------------------------------------
	
	void StreamBuffer::reallocate(const uint64_t byte_size)
	{
		CBN_Assert(!is_mapped(), "Cannot reallocate buffer while its mapped");

		bind();

		m_ByteSize = byte_size;
		glBufferData(static_cast<GLenum>(get_target()), byte_size, NULL, GL_DYNAMIC_DRAW);
	}

	//-------------------------------------------------------------------------------------

	void StreamBuffer::upload(const std::vector<uint8_t>& data, const uint64_t offset)
	{
		upload(data.data(), data.size(), offset);
	}

	//-------------------------------------------------------------------------------------

	void StreamBuffer::upload(const uint8_t* data, const uint64_t length, const uint64_t offset)
	{
		CBN_Assert(!is_mapped(), "Cannot reallocate buffer while its mapped");

		bind();
		glBufferSubData(static_cast<GLenum>(get_target()), offset, length, data);
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

	bool StreamBuffer::is_synchronized() const
	{
		return m_Synchronized;
	}



}