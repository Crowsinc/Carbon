#include "Texture.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------
	
	std::unordered_map<TextureUnit, GLint> Texture::s_BoundTexture2Ds;

	//-------------------------------------------------------------------------------------

	SRes<Texture> Texture::Create(const SRes<Image>& image, const TextureSettings& settings)
	{
		// Create the texture. This can only fail if the graphics device is out of memory
		// so we don't actually have to worry about validating any of the given parameters
		SRes<Texture> texture = Resource::WrapShared(new Texture(image, settings));

		// If we could not properly create the texture due to 
		// being out of memory, run the texture out of scope
		// so it is destroyed and fail creation
		if(glGetError() == GL_OUT_OF_MEMORY)
		{
			return nullptr;
		}

		return texture;
	}
	
	//-------------------------------------------------------------------------------------

	SRes<Texture> Texture::Open(const std::filesystem::path& path, const TextureSettings& settings)
	{
		// First open the image, making sure to flip on the y-axis since OpenGL 
		// has (0,0) on the bottom left while the image has (0,0) on the top left. 
		SRes<Image> image = Image::Open(path, true);
		if(!image)
			return nullptr;

		// Continue with the normal create function 
		return Create(image, settings);
	}

	//-------------------------------------------------------------------------------------

	GLint Texture::SupportedTextureUnits()
	{
		GLint texture_units;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
		return texture_units;
	}
	
	//-------------------------------------------------------------------------------------

	void Texture::upload_image_data(const Colour* data, const unsigned width, const unsigned height)
	{
		// We need to bind the image before we can update its contents
		bind();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	//-------------------------------------------------------------------------------------

	std::array<GLint, 4> Texture::create_swizzle_mask(const TextureSwizzle swizzle)
	{
		switch(swizzle)
		{
			case TextureSwizzle::RGBA:
				return std::array<GLint, 4>{GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA};
			case TextureSwizzle::BGRA:
				return std::array<GLint, 4>{GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA};
			case TextureSwizzle::RGB:
				return std::array<GLint, 4>{GL_RED, GL_GREEN, GL_BLUE, GL_ONE};
			case TextureSwizzle::BGR:
				return std::array<GLint, 4>{GL_BLUE, GL_GREEN, GL_RED, GL_ONE};
			default:
				return {};
		}
	}
	
	//-------------------------------------------------------------------------------------

	Texture::Texture(const SRes<Image>& image, const TextureSettings& settings)
		: m_Resolution(image->resolution())
	{
		// Create the texture
		glGenTextures(1, &m_TextureID);

		// Configure texture properties
		configure(settings);
		
		// Note that this can fail if the graphics device is out of memory,
		// however the error is generated by the device not OpenGL so it must 
		// be validated independently in the create function.
		upload_image_data(image->data(), image->width(), image->height());
	}
	
	//-------------------------------------------------------------------------------------

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_TextureID);
	}
	
	//-------------------------------------------------------------------------------------

	bool Texture::is_bound(const TextureUnit texture_unit) const
	{
		return m_TextureUnit == texture_unit && s_BoundTexture2Ds[texture_unit] == m_TextureID;
	}

	//-------------------------------------------------------------------------------------

	bool Texture::is_bound() const
	{
		return s_BoundTexture2Ds[m_TextureUnit] == m_TextureID;
	}

	//-------------------------------------------------------------------------------------

	void Texture::bind(const TextureUnit texture_unit) const
	{
		// Only bind the texture if it is not already 
		// bound to the given texture unit
		if(!is_bound(texture_unit))
		{
			// Update the texture binding states
			s_BoundTexture2Ds[texture_unit] = m_TextureID;
			m_TextureUnit = texture_unit;

			// Bind the texture to the correct unit
			glActiveTexture(GL_TEXTURE0 + static_cast<int>(texture_unit));
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
		}
	}


	//-------------------------------------------------------------------------------------

	void Texture::unbind() const
	{
		// Only unbind if the texture is actually bound in the first place
		if(is_bound())
		{
			// Unbind from the texture unit the texture is bound to
			glActiveTexture(GL_TEXTURE0 + static_cast<int>(m_TextureUnit));
			glBindTexture(GL_TEXTURE_2D, 0);

			// Reset state information
			s_BoundTexture2Ds[m_TextureUnit] = 0;
		}
	}

	//-------------------------------------------------------------------------------------

	void Texture::configure(const TextureSettings& settings)
	{
		// We need to bind the texture before we can change its parameters
		bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(settings.horizontal_wrapping));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(settings.vertical_wrapping));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(settings.minifying_filter));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(settings.magnifying_filter));
	
		// Update the swizzle mask
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, create_swizzle_mask(settings.swizzle).data());

		// Update the stored properties
		m_Settings = settings;
	}
	
	//-------------------------------------------------------------------------------------

	TextureSettings Texture::settings() const
	{
		return m_Settings;
	}
	
	//-------------------------------------------------------------------------------------

	const unsigned Texture::width() const
	{
		return m_Resolution.x;
	}
	
	//-------------------------------------------------------------------------------------

	const unsigned Texture::height() const
	{
		return m_Resolution.y;
	}
	
	//-------------------------------------------------------------------------------------

	glm::uvec2 Texture::resolution() const
	{
		return m_Resolution;
	}

	//-------------------------------------------------------------------------------------

	TextureUVMap Texture::uvs() const
	{
		// Texture uvs just encompass the entire texture, so we can just hardcode them
		return {{0.0f, 0.0f},
			    {0.0f, 1.0f},
			    {1.0f, 1.0f},
			    {1.0f, 0.0f}};
	}

	//-------------------------------------------------------------------------------------



}