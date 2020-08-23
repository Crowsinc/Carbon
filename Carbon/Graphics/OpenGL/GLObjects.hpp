#pragma once

#include "GLTypedObject.hpp"
#include "GLUntypedObject.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	struct VertexArrayObject : public GLUntypedObject<GL_VERTEX_ARRAY>
	{
		VertexArrayObject() : GLUntypedObject(glGenVertexArrays, glDeleteVertexArrays, glBindVertexArray) {}
	};


	//-------------------------------------------------------------------------------------

	struct VertexBuffer : public GLTypedObject<GL_BUFFER, GL_ARRAY_BUFFER>
	{
		VertexBuffer() : GLTypedObject(glGenBuffers, glDeleteBuffers, glBindBuffer) {}
	};

	//-------------------------------------------------------------------------------------

	struct IndexBuffer : public GLTypedObject<GL_BUFFER, GL_ELEMENT_ARRAY_BUFFER>
	{
		IndexBuffer() : GLTypedObject(glGenBuffers, glDeleteBuffers, glBindBuffer) {}
	};

	//-------------------------------------------------------------------------------------

	struct UniformBuffer : public GLTypedObject<GL_BUFFER, GL_UNIFORM_BUFFER>
	{
		UniformBuffer() : GLTypedObject(glGenBuffers, glDeleteBuffers, glBindBuffer) {}
	};

	//-------------------------------------------------------------------------------------
}