#pragma once

#include <unordered_map>
#include <array>

#include "../../Memory/Resource.hpp"
#include "../OpenGL/OpenGL.hpp"
#include "Image.hpp"

namespace cbn
{
	enum class TextureUnit : GLint
	{
		UNIT_0,
		UNIT_1,
		UNIT_2,
		UNIT_3,
		UNIT_4,
		UNIT_5,
		UNIT_6,
		UNIT_7,
		UNIT_8,
		UNIT_9,
		UNIT_10,
		UNIT_11,
		UNIT_12,
		UNIT_13,
		UNIT_14,
		UNIT_15,
		UNIT_16,
		UNIT_17,
		UNIT_18,
		UNIT_19,
		UNIT_20,
		UNIT_21,
		UNIT_22,
		UNIT_23,
		UNIT_24,
		UNIT_25,
		UNIT_26,
		UNIT_27,
		UNIT_28,
		UNIT_29,
		UNIT_30,
		UNIT_31,

	};

	enum class TextureSwizzle
	{
		RGBA,
		BGRA,
		RGB,
		BGR
	};

	enum class TextureFilter : GLint
	{
		NEAREST = GL_NEAREST,
		LINEAR = GL_LINEAR
	};

	enum class TextureWrapping : GLint
	{
		REPEAT = GL_REPEAT,
		REPEAT_MIRRORED = GL_MIRRORED_REPEAT,
		CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
		CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
	};

	struct TextureUVMap
	{
		glm::vec2 uv_1;
		glm::vec2 uv_2;
		glm::vec2 uv_3;
		glm::vec2 uv_4;
	};

	struct TextureSettings
	{
		TextureFilter magnifying_filter = TextureFilter::NEAREST;
		TextureFilter minifying_filter = TextureFilter::LINEAR;

		TextureWrapping horizontal_wrapping = TextureWrapping::CLAMP_TO_EDGE;
		TextureWrapping vertical_wrapping = TextureWrapping::CLAMP_TO_EDGE;

		TextureSwizzle swizzle = TextureSwizzle::RGBA;
	};

	class Texture 
	{
	public:

		static SRes<Texture> Create(const SRes<Image>& image, const TextureSettings& settings = {});

		static SRes<Texture> Open(const std::filesystem::path& path, const TextureSettings& settings = {});

		static GLint SupportedTextureUnits();

	private:

		static std::unordered_map<TextureUnit,GLint> s_BoundTexture2Ds;

		mutable TextureUnit m_TextureUnit;
		TextureSettings m_Settings;
		glm::uvec2 m_Resolution;
		GLuint m_TextureID;

		void upload_image_data(const Colour* data, const unsigned width, const unsigned height);

		std::array<GLint, 4> create_swizzle_mask(const TextureSwizzle swizzle);

		Texture(const SRes<Image>& image, const TextureSettings& settings);

	public:

		~Texture();

		void unbind() const;

		void bind(const TextureUnit texture_unit = TextureUnit::UNIT_0) const;

		bool is_bound(const TextureUnit texture_unit) const;
		
		bool is_bound() const;

		void configure(const TextureSettings& settings);

		TextureSettings settings() const;

		const unsigned width() const;

		const unsigned height() const;

		glm::uvec2 resolution() const;

		TextureUVMap uvs() const;

	};

}