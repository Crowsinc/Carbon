#pragma once

#include <array>

#include "../OpenGL/GLTypedObject.hpp"
#include "../OpenGL/OpenGL.hpp"

#include "../OpenGL/TextureParameters.hpp"
#include "../../Memory/Resource.hpp"
#include "Image.hpp"

namespace cbn
{

	class Texture : public GLTypedObject<GL_TEXTURE, GL_TEXTURE_2D> 
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

	private:

		Properties m_Properties;
		glm::uvec2 m_Resolution;

		void upload_image_data(const Image::Pixel* data, const unsigned width, const unsigned height);

		Texture(const SRes<Image>& image, const Properties& properties);

	public:

		void configure(const Properties& properties);

		Properties get_properties() const;

		const unsigned width() const;

		const unsigned height() const;

		glm::uvec2 resolution() const;

		UVMap uvs() const;
	};

}