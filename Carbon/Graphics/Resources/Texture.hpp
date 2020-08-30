#pragma once

#include <unordered_map>
#include <array>

#include "../OpenGL/TextureParameters.hpp"
#include "../../Memory/Resource.hpp"
#include "../OpenGL/OpenGL.hpp"
#include "Image.hpp"

namespace cbn
{

	class Texture 
	{
	public:

		struct Properties
		{
			TextureFilter magnifying_filter = TextureFilter::NEAREST;
			TextureFilter minifying_filter = TextureFilter::LINEAR;

			TextureWrapping horizontal_wrapping = TextureWrapping::REPEAT;
			TextureWrapping vertical_wrapping = TextureWrapping::REPEAT;

			TextureSwizzle swizzle = TextureSwizzle::RGBA;
		};

		struct UVMap
		{
			glm::vec2 ul;
			glm::vec2 ll;
			glm::vec2 lr;
			glm::vec2 ur;
		};

		static SRes<Texture> Create(const SRes<Image>& image, const Properties& properties);

		static SRes<Texture> Open(const std::filesystem::path& path, const Properties& properties);

		static GLint SupportedTextureUnits();

	private:

		static std::unordered_map<TextureUnit,GLint> s_BoundTextures;

		mutable TextureUnit m_TextureUnit;
		Properties m_Properties;
		glm::uvec2 m_Resolution;
		GLuint m_TextureID;

		void upload_image_data(const Image::Pixel* data, const unsigned width, const unsigned height);

		std::array<GLint, 4> create_swizzle_mask(const TextureSwizzle swizzle);

		Texture(const SRes<Image>& image, const Properties& properties);

	public:

		~Texture();

		void unbind() const;

		void bind(const TextureUnit texture_unit = TextureUnit::UNIT_0) const;

		bool is_bound(const TextureUnit texture_unit = TextureUnit::UNIT_0) const;

		void configure(const Properties& properties);

		Properties get_properties() const;

		const unsigned width() const;

		const unsigned height() const;

		glm::uvec2 resolution() const;

		UVMap uv_mapping() const;

	};

}