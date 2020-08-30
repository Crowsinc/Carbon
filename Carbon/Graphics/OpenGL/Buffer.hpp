#pragma once

#include <unordered_map>

#include "OpenGL.hpp"

namespace cbn
{

	class Buffer
	{
	public:

		enum class Target
		{
			ELEMENT_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
			UNIFORM_BUFFER = GL_UNIFORM_BUFFER,
			VERTEX_BUFFER = GL_ARRAY_BUFFER,
		};

	private:

		static std::unordered_map<Target, GLint> m_BoundBuffers;

		const Target m_Target;
		GLuint m_BufferID;

	public:

		Buffer(const Target target);

		~Buffer();

		void bind();

		void unbind();

		bool is_bound() const;

		Target get_target() const;

	};

}