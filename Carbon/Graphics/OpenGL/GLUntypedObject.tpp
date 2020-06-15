#include "GLUntypedObject.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID>
	GLuint GLUntypedObject<ObjectTypeID>::s_BoundObjectID = 0;

	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID>
	inline GLUntypedObject<ObjectTypeID>::GLUntypedObject(GeneratorFunction generator, DeleterFunction deleter, BinderFunction binder)
		: m_DeleteFunction(deleter),
		m_BindFunction(binder),
		m_ObjectID(0)
	{
		// Generate the object
		generator(1, &m_ObjectID);
	}

	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID>
	inline GLUntypedObject<ObjectTypeID>::~GLUntypedObject()
	{
		// Make sure we destroy the object to avoid memory leaks
		m_DeleteFunction(1, &m_ObjectID);
	}

	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID>
	inline void GLUntypedObject<ObjectTypeID>::bind() const
	{
		// Only bind the object if it is not already bound
		if(!is_bound())
		{
			m_BindFunction(m_ObjectID);
			s_BoundObjectID = m_ObjectID;
		}
	}

	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID>
	inline void GLUntypedObject<ObjectTypeID>::unbind() const
	{
		// Only unbind the object if it is actually bound
		if(is_bound())
		{
			// Bind to 0 in order to unbind the object
			m_BindFunction(0);
			s_BoundObjectID = 0;
		}
	}

	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID>
	inline bool GLUntypedObject<ObjectTypeID>::is_bound() const
	{
		return m_ObjectID == s_BoundObjectID;
	}

	//-------------------------------------------------------------------------------------

}