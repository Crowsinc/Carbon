#pragma once

#include "OpenGL.hpp"

namespace cbn
{
	template<GLenum ObjectTypeID>
	class GLUntypedObject
	{
	public:
		typedef void(*GeneratorFunction)(GLsizei, GLuint*);
		typedef void(*DeleterFunction)(GLsizei, const GLuint*);
		typedef void(*BinderFunction)(GLuint);

	private:

		DeleterFunction m_DeleteFunction;
		BinderFunction m_BindFunction;
		static GLuint s_BoundObjectID;
		GLuint m_ObjectID;

	public:

		GLUntypedObject(GeneratorFunction generator, DeleterFunction deleter, BinderFunction binder);

		~GLUntypedObject();

		void bind() const;

		void unbind() const;

		bool is_bound() const;
	};
}

#include "GLUntypedObject.tpp"