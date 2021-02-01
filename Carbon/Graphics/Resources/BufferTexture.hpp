#pragma once

#include <vector>
#include <array>

#include "Texture.hpp"
#include "../OpenGL/OpenGL.hpp"
#include "../../Memory/Resource.hpp"
#include "../../Utility/Version.hpp"

namespace cbn
{
	enum class BufferTextureDataFormat : GLenum
	{
		VEC1_32U = GL_R32UI,
		VEC2_32U = GL_RG32UI,
		VEC3_32U = GL_RGB32UI,
		VEC4_32U = GL_RGBA32UI,
		VEC1_16U = GL_R16UI,
		VEC2_16U = GL_RG16UI,
		VEC3_16U = GL_RGB16UI,
		VEC4_16U = GL_RGBA16UI,
		VEC1_8U = GL_R8UI,
		VEC2_8U = GL_RG8UI,
		VEC3_8U = GL_RGB8UI,
		VEC4_8U = GL_RGBA8UI,
		VEC1_32I = GL_R32I,
		VEC2_32I = GL_RG32I,
		VEC3_32I = GL_RGB32I,
		VEC4_32I = GL_RGBA32I,
		VEC1_16I = GL_R16I,
		VEC2_16I = GL_RG16I,
		VEC3_16I = GL_RGB16I,
		VEC4_16I = GL_RGBA16I,
		VEC1_8I = GL_R8I,
		VEC2_8I = GL_RG8I,
		VEC3_8I = GL_RGB8I,
		VEC4_8I = GL_RGBA8I,
		VEC1_F = GL_R32F,
		VEC2_F = GL_RG32F,
		VEC3_F = GL_RGB32F,
		VEC4_F = GL_RGBA32F,
	};

	class BufferTexture
	{
	public:

		static SRes<BufferTexture> Allocate(const std::vector<uint8_t>& data, const BufferTextureDataFormat data_format, const Version& opengl_version);
		
		static SRes<BufferTexture> Allocate(const uint8_t* data, const uint64_t length, const BufferTextureDataFormat data_format, const Version& opengl_version);

	private:

		static std::array<GLuint, 32> s_BoundBufferTextures;
		
		mutable TextureUnit m_TextureUnit;
		const BufferTextureDataFormat m_Format;
		GLuint m_BufferID, m_TextureID;
		const uint64_t m_ByteSize;

		BufferTexture(const uint64_t byte_size, const BufferTextureDataFormat data_format);

	public:

		~BufferTexture();

		void unbind() const;

		void bind(const TextureUnit texture_unit = TextureUnit::UNIT_0) const;

		void force_bind(const TextureUnit texture_unit = TextureUnit::UNIT_0) const;

		bool is_bound(const TextureUnit texture_unit) const;
		
		bool is_bound() const;
		
		uint64_t size() const;

		BufferTextureDataFormat format() const;
	};

}
