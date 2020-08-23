#pragma once

#include <array>

#include "../OpenGL/GLTypedObject.hpp"
#include "../OpenGL/OpenGL.hpp"

#include "../OpenGL/TextureParameters.hpp"
#include "../../Memory/Resource.hpp"
#include "../../Utility/Enum.hpp"
#include "Image.hpp"

namespace cbn
{

	class Texture 
	{
	public:

		struct Properties
		{
			Filter magnifying_filter = Filter::NEAREST;
			Filter minifying_filter = Filter::LINEAR;

			Wrapping horizontal_wrapping = Wrapping::REPEAT;
			Wrapping vertical_wrapping = Wrapping::REPEAT;

			Swizzle swizzle = Swizzle::RGBA;
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

		static std::array<GLint, static_cast<size_t>(TopUnit)> s_BoundTexturesByUnit;

		mutable Enum<TextureUnit> m_TextureUnit;
		Properties m_Properties;
		glm::uvec2 m_Resolution;
		GLuint m_TextureID;

		void upload_image_data(const Image::Pixel* data, const unsigned width, const unsigned height);

		Texture(const SRes<Image>& image, const Properties& properties);

	public:

		~Texture();

		void unbind() const;

		void bind(const Enum<TextureUnit> texture_unit = TextureUnit::UNIT_0) const;

		bool is_bound(const Enum<TextureUnit> texture_unit = TextureUnit::UNIT_0) const;

		void configure(const Properties& properties);

		Properties get_properties() const;

		const unsigned width() const;

		const unsigned height() const;

		glm::uvec2 resolution() const;

		UVMap uv_mapping() const;

	};

}