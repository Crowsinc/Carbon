#pragma once

#include <unordered_map>

#include "OpenGL.hpp"

namespace cbn
{

	enum class BufferTarget
	{
		ELEMENT_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
		UNIFORM_BUFFER = GL_UNIFORM_BUFFER,
		VERTEX_BUFFER = GL_ARRAY_BUFFER,
	};

	class Buffer
	{
	private:

		static std::unordered_map<BufferTarget, GLint> s_BoundBuffers;

		const BufferTarget m_Target;
		GLuint m_BufferID;

	public:

		Buffer(const BufferTarget target);

		~Buffer();

		void bind();

		void unbind();

		void force_bind();

		bool is_bound() const;

		BufferTarget get_target() const;

	};

}