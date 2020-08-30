#include "VertexArrayObject.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	GLuint VertexArrayObject::s_BoundObjectID = 0;

	//-------------------------------------------------------------------------------------

	inline VertexArrayObject::VertexArrayObject()
		: m_ObjectID(0)
	{
		glGenVertexArrays(1, &m_ObjectID);
	}

	//-------------------------------------------------------------------------------------

	VertexArrayObject::~VertexArrayObject()
	{
		glDeleteVertexArrays(1, &m_ObjectID);
	}

	//-------------------------------------------------------------------------------------

	void VertexArrayObject::bind() const
	{
		// Only bind the object if it is not already bound
		if(!is_bound())
		{
			s_BoundObjectID = m_ObjectID;
			glBindVertexArray(m_ObjectID);
		}
	}

	//-------------------------------------------------------------------------------------

	void VertexArrayObject::unbind() const
	{
		// Only unbind the object if it is actually bound
		if(is_bound())
		{
			s_BoundObjectID = 0;
			glBindVertexArray(0);
		}
	}

	//-------------------------------------------------------------------------------------

	bool VertexArrayObject::is_bound() const
	{
		return m_ObjectID == s_BoundObjectID;
	}
	
	//-------------------------------------------------------------------------------------

}