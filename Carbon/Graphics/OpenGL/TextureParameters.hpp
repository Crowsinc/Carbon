#pragma once

#include "OpenGL.hpp"

#include <array>
#include <unordered_map>

namespace cbn
{
	enum class TextureUnit : GLint
	{
		UNIT_0 = 0,
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
	constexpr TextureUnit TopUnit = TextureUnit::UNIT_15;

	enum class Swizzle
	{
		RGBA,
		BGRA,
		RGB,
		BGR
	};
	
	const std::unordered_map<Swizzle, const std::array<GLint,4>> SwizzleMasks = 
	{
		{Swizzle::RGBA, std::array<GLint, 4>{GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}},
		{Swizzle::BGRA, std::array<GLint, 4>{GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA}},
		{Swizzle::RGB, std::array<GLint, 4>{GL_RED, GL_GREEN, GL_BLUE, GL_ONE}},
		{Swizzle::BGR, std::array<GLint, 4>{GL_BLUE, GL_GREEN, GL_RED, GL_ONE}}
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

}