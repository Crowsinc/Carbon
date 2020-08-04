#pragma once

#include "OpenGL.hpp"

#include <array>
#include <unordered_map>

namespace cbn
{

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