#include "GLTypedObject.hpp"

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
	GLTypedObject<ObjectTypeID, Type>::GLTypedObject(GLTypedObject&& object)
		: m_DeleteFunction(object.m_DeleteFunction),
		m_BindFunction(object.m_BindFunction),
		m_ObjectID(object.m_ObjectID)
	{
		// Remove the delete function of the other object so 
		// that it doesnt invalidate our object when it goes out of scope
		object.m_DeleteFunction = nullptr;
	}
	
	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID, GLenum Type>
	inline GLTypedObject<ObjectTypeID, Type>::~GLTypedObject()
	{
		// Make sure we destroy the object to avoid memory leaks
		// Only do this if a delete function is given, this allows
		// the object to be able to control when exactly the delete is called
		if(m_DeleteFunction != nullptr)
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

	template<GLenum ObjectTypeID, GLenum Type>
	GLuint GLTypedObject<ObjectTypeID, Type>::id() const
	{
		return m_ObjectID;
	}
	
	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID, GLenum Type>
	void GLTypedObject<ObjectTypeID, Type>::operator=(GLTypedObject&& object)
	{
		m_ObjectID = object.m_ObjectID;
		m_BindFunction = object.m_BindFunction;
		m_DeleteFunction = object.m_DeleteFunction;
	
		// Remove the delete function of the other object so 
		// that it doesnt invalidate our object when it goes out of scope
		object.m_DeleteFunction = nullptr;
	}

	//-------------------------------------------------------------------------------------
}