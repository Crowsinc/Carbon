#include "StaticBuffer.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	SRes<StaticBuffer> StaticBuffer::Allocate(const std::vector<uint8_t>& data, const BufferTarget target, const Version opengl_version)
	{
		return Allocate(data.data(), data.size(), target, opengl_version);
	}

	//-------------------------------------------------------------------------------------

	SRes<StaticBuffer> StaticBuffer::Allocate(const uint8_t* data, const uint64_t size, const BufferTarget target, const Version opengl_version)
	{
		// Create the buffer first to generate the OpenGL buffer object, then simply attach the required memory to it
		auto static_buffer = SRes<StaticBuffer>(new StaticBuffer(target, size));

		static_buffer->bind();

		// If OpenGL 4.4 is supported, create immutable storage otherwise use the normal buffer data.
		if(opengl_version >= Version{4,4})
		{
			glBufferStorage(static_cast<GLenum>(target), size, data, NULL);
		}
		else
		{
			glBufferData(static_cast<GLenum>(target), size, data, GL_STATIC_DRAW);
		}

		static_buffer->unbind();

		return static_buffer;
	}

	//-------------------------------------------------------------------------------------

	StaticBuffer::StaticBuffer(const BufferTarget target, const uint64_t byte_size)
		: Buffer(target),
		m_ByteSize(byte_size) {}

	//-------------------------------------------------------------------------------------

	uint64_t StaticBuffer::size() const
	{
		return m_ByteSize;
	}

	//-------------------------------------------------------------------------------------
	

}