#pragma once

#include "OpenGL.hpp"

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

}