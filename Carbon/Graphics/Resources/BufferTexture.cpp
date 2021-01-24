#include "BufferTexture.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	std::unordered_map<TextureUnit, GLint> BufferTexture::s_BoundBufferTextures;

	//-------------------------------------------------------------------------------------

	SRes<BufferTexture> BufferTexture::Allocate(const std::vector<uint8_t>& data, const BufferTextureDataFormat data_format, const Version& opengl_version)
	{
		return Allocate(data.data(), data.size(), data_format, opengl_version);
	}

	//-------------------------------------------------------------------------------------

	SRes<BufferTexture> BufferTexture::Allocate(const uint8_t* data, const uint64_t length, const BufferTextureDataFormat data_format, const Version& opengl_version)
	{
		// Create the texture buffer first to generate the OpenGL buffer object, then simply attach the required memory to it
		auto texture_buffer = SRes<BufferTexture>(new BufferTexture(length, data_format));

		glBindBuffer(GL_TEXTURE_BUFFER, texture_buffer->m_BufferID);

		// If OpenGL 4.4 is supported, create immutable storage otherwise use the normal buffer data.
		if(opengl_version >= Version{4,4})
		{
			glBufferStorage(GL_TEXTURE_BUFFER, length, data, NULL);
		}
		else
		{
			glBufferData(GL_TEXTURE_BUFFER, length, data, GL_STATIC_DRAW);
		}

		glBindBuffer(GL_TEXTURE_BUFFER, texture_buffer->m_BufferID);

		return texture_buffer;
	}

	//-------------------------------------------------------------------------------------

	BufferTexture::BufferTexture(const uint64_t byte_size, const BufferTextureDataFormat data_format)
		: m_ByteSize(byte_size),
		m_Format(data_format)
	{
		// Create the buffer and texture, then bind them together
		glGenBuffers(1, &m_BufferID);
		glGenTextures(1, &m_TextureID);
		
		// Bind the texture and buffer
		force_bind();
		glBindBuffer(GL_TEXTURE_BUFFER, m_BufferID);
		glTexBuffer(GL_TEXTURE_BUFFER, static_cast<GLenum>(data_format), m_BufferID);
	}

	//-------------------------------------------------------------------------------------

	BufferTexture::~BufferTexture()
	{
		// Before we destroy the object, we need to ensure the static bounded object tracker
		// doesnt consider it as being bound. Otherwise issues will arise when a new object
		// takes its ID.
		unbind();
		glDeleteBuffers(1, &m_BufferID);
		glDeleteTextures(1, &m_TextureID);
	}
	
	//-------------------------------------------------------------------------------------

	void BufferTexture::unbind() const
	{
		// Only unbind if the texture is actually bound in the first place
		if(is_bound())
		{
			// Unbind from the texture unit the texture is bound to
			glActiveTexture(GL_TEXTURE0 + static_cast<int>(m_TextureUnit));
			glBindTexture(GL_TEXTURE_BUFFER, 0);

			// Reset state information
			s_BoundBufferTextures[m_TextureUnit] = 0;
		}
	}
	
	//-------------------------------------------------------------------------------------

	void BufferTexture::bind(const TextureUnit texture_unit) const
	{
		// Only bind if it is not already bound to the given texture unit
		if(!is_bound(texture_unit))
		{
			force_bind(texture_unit);
		}
	}
	
	//-------------------------------------------------------------------------------------

	void BufferTexture::force_bind(const TextureUnit texture_unit) const
	{
		// Update the buffer texture binding states
		s_BoundBufferTextures[texture_unit] = m_TextureID;
		m_TextureUnit = texture_unit;

		// Bind the buffer texture to the correct unit
		glActiveTexture(GL_TEXTURE0 + static_cast<int>(texture_unit));
		glBindTexture(GL_TEXTURE_BUFFER, m_TextureID);
	}
	
	//-------------------------------------------------------------------------------------

	bool BufferTexture::is_bound(const TextureUnit texture_unit) const
	{
		return m_TextureUnit == texture_unit && s_BoundBufferTextures[texture_unit] == m_TextureID;
	}
	
	//-------------------------------------------------------------------------------------

	bool BufferTexture::is_bound() const
	{
		return s_BoundBufferTextures[m_TextureUnit] == m_TextureID;
	}

	//-------------------------------------------------------------------------------------
	
	uint64_t BufferTexture::size() const
	{
		return m_ByteSize;
	}
	
	//-------------------------------------------------------------------------------------

	BufferTextureDataFormat BufferTexture::format() const
	{
		return m_Format;
	}

	//-------------------------------------------------------------------------------------
}