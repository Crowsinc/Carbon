#include "StaticBuffer.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	SRes<StaticBuffer> StaticBuffer::Allocate(const std::vector<uint8_t>& data, const Target target, const Version opengl_version)
	{
		// Create the buffer first to generate the OpenGL buffer object, then simply attach the required memory to it
		auto static_buffer = SRes<StaticBuffer>(new StaticBuffer(target, data.size()));

		static_buffer->bind();

		// If OpenGL 4.4 is supported, create immutable storage otherwise use the normal buffer data.
		if(opengl_version >= Version{4,4})
		{
			glBufferStorage(static_cast<GLenum>(target), data.size(), data.data(), NULL);
		}
		else
		{
			glBufferData(static_cast<GLenum>(target), data.size(), data.data(), GL_STATIC_DRAW);
		}

		return static_buffer;
	}

	//-------------------------------------------------------------------------------------

	StaticBuffer::StaticBuffer(const Buffer::Target target, const uint64_t byte_size)
		: Buffer(target),
		m_ByteSize(byte_size) {}

	//-------------------------------------------------------------------------------------

	uint64_t StaticBuffer::size() const
	{
		return m_ByteSize;
	}

	//-------------------------------------------------------------------------------------
	

}