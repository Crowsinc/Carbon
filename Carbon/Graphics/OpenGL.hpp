#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace cbn
{

	//-------------------------------------------------------------------------------------

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

	//-------------------------------------------------------------------------------------

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

	//-------------------------------------------------------------------------------------

	struct VertexArrayObject : public GLUntypedObject<GL_VERTEX_ARRAY>
	{
		VertexArrayObject() : GLUntypedObject(glGenVertexArrays, glDeleteVertexArrays, glBindVertexArray) {}
	};


	//-------------------------------------------------------------------------------------

	struct VertexBuffer : public GLTypedObject<GL_BUFFER,GL_ARRAY_BUFFER>
	{
		VertexBuffer() : GLTypedObject(glGenBuffers, glDeleteBuffers, glBindBuffer) {}
	};

	//-------------------------------------------------------------------------------------

	struct IndexBuffer : public GLTypedObject<GL_BUFFER, GL_ELEMENT_ARRAY_BUFFER>
	{
		IndexBuffer() : GLTypedObject(glGenBuffers, glDeleteBuffers, glBindBuffer) {}
	};

	//-------------------------------------------------------------------------------------

}

#include "OpenGL.tpp"