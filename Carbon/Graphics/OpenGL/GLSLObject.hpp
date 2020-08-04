#pragma once

#include "OpenGL.hpp"
#include "GLUntypedObject.hpp"

namespace cbn
{
	template<GLenum ObjectTypeID>
	class GLSLObject : public GLUntypedObject<ObjectTypeID>
	{
	public:
		
		typedef GLuint(*EnumGeneratorFunction)(GLenum);
		typedef GLuint(*GeneratorFunction)();

		typedef void(*DeleterFunction)(GLuint);
		typedef void(*BinderFunction)(GLuint);

	private:

		DeleterFunction m_DeleteFunction;

		static void empty_generator(GLsizei, GLuint*);

	public:

		GLSLObject(EnumGeneratorFunction generator, const GLenum enum_value, DeleterFunction deleter, BinderFunction binder);

		GLSLObject(GeneratorFunction generator, DeleterFunction deleter, BinderFunction binder);

		~GLSLObject();
	};
}

#include "GLSLObject.tpp"