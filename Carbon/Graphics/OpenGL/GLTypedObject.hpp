#pragma once

#include "OpenGL.hpp"

namespace cbn
{
	template<GLenum ObjectTypeID, GLenum Type>
	class GLTypedObject
	{
	public:

		typedef void(*GeneratorFunction)(GLsizei, GLuint*);
		typedef void(*DeleterFunction)(GLsizei, const GLuint*);
		typedef void(*BinderFunction)(GLenum, GLuint);

	private:

		DeleterFunction m_DeleteFunction;
		BinderFunction m_BindFunction;
		static GLuint s_BoundObjectID;
		GLuint m_ObjectID;

	public:

		GLTypedObject(GeneratorFunction generator, DeleterFunction deleter, BinderFunction binder);

		~GLTypedObject();

		void bind() const;

		void unbind() const;

		bool is_bound() const;
	};
}

#include "GLTypedObject.tpp"