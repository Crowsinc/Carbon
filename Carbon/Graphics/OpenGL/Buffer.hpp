#pragma once

#include <array>

#include "OpenGL.hpp"

namespace cbn
{

	enum class BufferTarget : GLenum
	{
		ELEMENT_BUFFER,
		UNIFORM_BUFFER,
		VERTEX_BUFFER,
	};

	class Buffer
	{
	private:

		static std::array<GLuint, 3> s_BoundBuffers;

		const BufferTarget m_Target;
		GLuint m_BufferID;

	protected:

		static GLenum to_opengl_target(const BufferTarget& target);

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