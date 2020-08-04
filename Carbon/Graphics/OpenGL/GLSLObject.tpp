#include "GLSLObject.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID>
	void GLSLObject<ObjectTypeID>::empty_generator(GLsizei, GLuint*) {}
	
	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID>
	inline GLSLObject<ObjectTypeID>::GLSLObject(EnumGeneratorFunction generator,const GLenum enum_value, DeleterFunction deleter, BinderFunction binder)
		: GLUntypedObject<ObjectTypeID>(empty_generator, nullptr, binder),
		m_DeleteFunction(deleter)
	{
		this->m_ObjectID = generator(enum_value);
	}
	
	//-------------------------------------------------------------------------------------
	
	template<GLenum ObjectTypeID>
	inline GLSLObject<ObjectTypeID>::GLSLObject(GeneratorFunction generator, DeleterFunction deleter, BinderFunction binder)
		: GLUntypedObject<ObjectTypeID>(empty_generator, nullptr, binder),
		m_DeleteFunction(deleter)
	{
		this->m_ObjectID = generator();
	}

	//-------------------------------------------------------------------------------------

	template<GLenum ObjectTypeID>
	GLSLObject<ObjectTypeID>::~GLSLObject()
	{
		if(m_DeleteFunction != nullptr)
			m_DeleteFunction(this->m_ObjectID);
	}

	//-------------------------------------------------------------------------------------
}