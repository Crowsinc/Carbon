#pragma once

#include "OpenGL.hpp"

namespace cbn
{
	//TODO: this class has no protection for copies
	class VertexArrayObject
	{
	private:

		static GLuint s_BoundObjectID;

		GLuint m_ObjectID;

	public:

		VertexArrayObject();

		~VertexArrayObject();

		void bind() const;

		void unbind() const;

		bool is_bound() const;

	};
}

