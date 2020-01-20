#include "OpenGL.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID, GLenum Type>
	GLuint GLTypedObject<ObjectTypeID, Type>::s_BoundObjectID = 0;

	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID, GLenum Type>
	inline GLTypedObject<ObjectTypeID, Type>::GLTypedObject(GeneratorFunction generator, DeleterFunction deleter, BinderFunction binder)
		: m_DeleteFunction(deleter),
		m_BindFunction(binder),
		m_ObjectID(0)
	{
		// Generate the object
		generator(1, &m_ObjectID);
	}

	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID, GLenum Type>
	inline GLTypedObject<ObjectTypeID, Type>::~GLTypedObject()
	{
		// Make sure we destroy the object to avoid memory leaks
		m_DeleteFunction(1, &m_ObjectID);
	}

	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID, GLenum Type>
	inline void GLTypedObject<ObjectTypeID, Type>::bind() const
	{
		// Only bind the object if it is not already bound
		if(!is_bound())
		{
			m_BindFunction(Type, m_ObjectID);
			s_BoundObjectID = m_ObjectID;
		}
	}
	
	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID, GLenum Type>
	inline void GLTypedObject<ObjectTypeID, Type>::unbind() const
	{
		// Only unbind the object if it is actually bound
		if(is_bound())
		{
			// Bind to 0 in order to unbind the object
			m_BindFunction(Type, 0);
			s_BoundObjectID = 0;
		}
	}
	
	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID, GLenum Type>
	inline bool GLTypedObject<ObjectTypeID, Type>::is_bound() const
	{
		return m_ObjectID == s_BoundObjectID;
	}
	
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