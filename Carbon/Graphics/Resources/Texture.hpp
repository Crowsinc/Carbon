#pragma once

#include <array>

#include "../OpenGL/GLTypedObject.hpp"
#include "../OpenGL/OpenGL.hpp"

#include "../../Utility/Resource.hpp"
#include "Image.hpp"

namespace cbn
{

	class Texture : public GLTypedObject<GL_TEXTURE, GL_TEXTURE_2D> 
	{
	public:

		enum class SwizzleMask
		{
			RGBA,
			BGRA,
			RGB,
			BGR
		};

		enum class Filter : GLint
		{
			NEAREST = GL_NEAREST,
			LINEAR = GL_LINEAR
		};

		enum class Wrapping : GLint
		{
			REPEAT = GL_REPEAT,
			REPEAT_MIRRORED = GL_MIRRORED_REPEAT,
			CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
			CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
		};

		struct Properties
		{
			Filter magnifying_filter = Filter::LINEAR;
			Filter minifying_filter = Filter::LINEAR;

			Wrapping horizontal_wrapping = Wrapping::REPEAT;
			Wrapping vertical_wrapping = Wrapping::REPEAT;

			SwizzleMask swizzle_mask = SwizzleMask::RGBA;
		};

		static Res<Texture> Create(const Image& image, const Properties& properties);

		static Res<Texture> Open(const std::filesystem::path& path, const Properties& properties);

	private:

		Properties m_Properties;
		glm::uvec2 m_Resolution;

		void upload_image_data(const Image::Pixel* data, const unsigned width, const unsigned height);

		std::array<GLint, 4> generate_swizzle_mask(const SwizzleMask swizzle_type) const;

		Texture(const Image& image, const Properties& properties);

		static void Delete(Texture& texture);

	public:

		Texture(const Texture& texture) = delete;

		Texture(Texture&& texture) noexcept;

		void configure(const Properties& properties);

		Properties get_properties() const;

		const unsigned width() const;

		const unsigned height() const;

		glm::uvec2 resolution() const;

		void operator=(Texture&& texture);
	};

}