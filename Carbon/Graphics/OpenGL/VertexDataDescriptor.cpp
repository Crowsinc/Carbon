#include "VertexDataDescriptor.hpp"

#include "../../Diagnostics/Assert.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	GLint VertexDataDescriptor::to_count(PackingType packing)
	{
		switch(packing)
		{
			case PackingType::SCALAR:
				return 1;

			case PackingType::VEC_2:
				return 2;

			case PackingType::VEC_3:
				return 3;

			case PackingType::VEC_4:
				return 4;

			default:
				CBN_Assert(false, "Unimplemented Vertex Data Packing Type");
				return 0;
		}
	}
	
	//-------------------------------------------------------------------------------------

	GLenum VertexDataDescriptor::to_opengl_type(DataType type)
	{
		// Convert data type to its opengl type
		switch(type)
		{
			case DataType::BYTE:
				return GL_BYTE;
			
			case DataType::UBYTE:
				return GL_UNSIGNED_BYTE;
			
			case DataType::SHORT:
				return GL_SHORT;
			
			case DataType::USHORT:
				return GL_UNSIGNED_SHORT;
			
			case DataType::INT:
				return GL_INT;
		
			case DataType::UINT:
			case DataType::BOOL:
				return GL_UNSIGNED_INT;
		
			case DataType::FLOAT:
				return GL_FLOAT;
			
			case DataType::DOUBLE:
				return GL_DOUBLE;

			case DataType::INT_2_10_10_10:
				return GL_INT_2_10_10_10_REV;

			case DataType::UINT_2_10_10_10:
				return GL_UNSIGNED_INT_2_10_10_10_REV;

			case DataType::INT_10F_11F_11F:
				return GL_UNSIGNED_INT_10F_11F_11F_REV;

			default:
				CBN_Assert(false, "Unimplemented Vertex Data Type");
				return 0;
		}
	}
	
	//-------------------------------------------------------------------------------------

	GLint VertexDataDescriptor::get_byte_size(GLenum opengl_type, GLint count)
	{
		// Convert type to its byte size
		GLint byte_size = 0;
		switch(opengl_type)
		{
			case GL_BYTE:
			case GL_UNSIGNED_BYTE:
			{
				byte_size = sizeof(uint8_t);
				break;
			}
			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
			{
				byte_size = sizeof(uint16_t);
				break;
			}
			case GL_INT:
			case GL_UNSIGNED_INT:
			case GL_INT_2_10_10_10_REV:
			case GL_UNSIGNED_INT_2_10_10_10_REV:
			case GL_UNSIGNED_INT_10F_11F_11F_REV:
			{
				byte_size = sizeof(uint32_t);
				break;
			}
			case GL_FIXED:
			case GL_FLOAT:
			{
				byte_size = sizeof(float);
				break;
			}
			case GL_HALF_FLOAT:
			{
				byte_size = sizeof(float) / 2;
				break;
			}
			case GL_DOUBLE:
			{
				byte_size = sizeof(double);
				break;
			}
		}

		// The total byte size will be the type byte size multiplied by the amount of that type
		return byte_size * count;
	}
	
	//-------------------------------------------------------------------------------------

	std::vector<VertexDataDescriptor::AttributeDescriptor> VertexDataDescriptor::get_attribute_descriptors() const
	{
		return m_AttributeDescriptors;
	}
	
	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(DataType type, PackingType packing, bool should_normalise)
	{
		AttributeDescriptor descriptor;
		descriptor.normalize = should_normalise;
		descriptor.type = to_opengl_type(type);
		descriptor.count = to_count(packing);
		descriptor.byte_size = get_byte_size(descriptor.type, descriptor.count);

		m_AttributeDescriptors.push_back(descriptor);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(bool attribute, bool should_normalise)
	{
		add_attribute(DataType::BOOL, PackingType::SCALAR, should_normalise);
	}
	
	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(uint8_t attribute, bool should_normalise)
	{
		add_attribute(DataType::UBYTE, PackingType::SCALAR, should_normalise);
	}
	
	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(int8_t attribute, bool should_normalise)
	{
		add_attribute(DataType::BYTE, PackingType::SCALAR, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(uint16_t attribute, bool should_normalise)
	{
		add_attribute(DataType::USHORT, PackingType::SCALAR, should_normalise);
	}
	
	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(int16_t attribute, bool should_normalise)
	{
		add_attribute(DataType::SHORT, PackingType::SCALAR, should_normalise);
	}
	
	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(uint32_t attribute, bool should_normalise)
	{
		add_attribute(DataType::UINT, PackingType::SCALAR, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(int32_t attribute, bool should_normalise)
	{
		add_attribute(DataType::INT, PackingType::SCALAR, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(float attribute, bool should_normalise)
	{
		add_attribute(DataType::FLOAT, PackingType::SCALAR, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(double attribute, bool should_normalise)
	{
		add_attribute(DataType::DOUBLE, PackingType::SCALAR, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::vec2 attribute, bool should_normalise)
	{
		add_attribute(DataType::FLOAT, PackingType::VEC_2, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::vec3 attribute, bool should_normalise)
	{
		add_attribute(DataType::FLOAT, PackingType::VEC_3, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::vec4 attribute, bool should_normalise)
	{
		add_attribute(DataType::FLOAT, PackingType::VEC_4, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::uvec2 attribute, bool should_normalise)
	{
		add_attribute(DataType::UINT, PackingType::VEC_2, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::uvec3 attribute, bool should_normalise)
	{
		add_attribute(DataType::UINT, PackingType::VEC_3, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::uvec4 attribute, bool should_normalise)
	{
		add_attribute(DataType::UINT, PackingType::VEC_4, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::u8vec2 attribute, bool should_normalise)
	{
		add_attribute(DataType::UBYTE, PackingType::VEC_2, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::u8vec3 attribute, bool should_normalise)
	{
		add_attribute(DataType::UBYTE, PackingType::VEC_3, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::u8vec4 attribute, bool should_normalise)
	{
		add_attribute(DataType::UBYTE, PackingType::VEC_4, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::u16vec2 attribute, bool should_normalise)
	{
		add_attribute(DataType::USHORT, PackingType::VEC_2, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::u16vec3 attribute, bool should_normalise)
	{
		add_attribute(DataType::USHORT, PackingType::VEC_3, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::u16vec4 attribute, bool should_normalise)
	{
		add_attribute(DataType::USHORT, PackingType::VEC_4, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::ivec2 attribute, bool should_normalise)
	{
		add_attribute(DataType::INT, PackingType::VEC_2, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::ivec3 attribute, bool should_normalise)
	{
		add_attribute(DataType::INT, PackingType::VEC_3, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::ivec4 attribute, bool should_normalise)
	{
		add_attribute(DataType::INT, PackingType::VEC_4, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::i8vec2 attribute, bool should_normalise)
	{
		add_attribute(DataType::BYTE, PackingType::VEC_2, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::i8vec3 attribute, bool should_normalise)
	{
		add_attribute(DataType::BYTE, PackingType::VEC_3, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::i8vec4 attribute, bool should_normalise)
	{
		add_attribute(DataType::BYTE, PackingType::VEC_4, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::i16vec2 attribute, bool should_normalise)
	{
		add_attribute(DataType::SHORT, PackingType::VEC_2, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::i16vec3 attribute, bool should_normalise)
	{
		add_attribute(DataType::SHORT, PackingType::VEC_3, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::i16vec4 attribute, bool should_normalise)
	{
		add_attribute(DataType::SHORT, PackingType::VEC_4, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::bvec2 attribute, bool should_normalise)
	{
		add_attribute(DataType::BOOL, PackingType::VEC_2, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::bvec3 attribute, bool should_normalise)
	{
		add_attribute(DataType::BOOL, PackingType::VEC_3, should_normalise);
	}

	//-------------------------------------------------------------------------------------

	void VertexDataDescriptor::add_attribute(glm::bvec4 attribute, bool should_normalise)
	{
		add_attribute(DataType::BOOL, PackingType::VEC_4, should_normalise);
	}


	//-------------------------------------------------------------------------------------

}