#pragma once

#include <vector>

#include "OpenGL.hpp"
#include <glm/glm.hpp>

namespace cbn
{

	class VertexDataDescriptor
	{
	public:

		enum class DataType
		{
			BOOL,
			BYTE,
			UBYTE,
			SHORT,
			USHORT,
			INT,
			UINT,
			FLOAT,
			DOUBLE,
			INT_2_10_10_10,
			UINT_2_10_10_10,
			INT_10F_11F_11F,
		};

		enum class PackingType
		{
			SCALAR,
			VEC_2,
			VEC_3,
			VEC_4,
		};

		struct AttributeDescriptor
		{
			GLboolean normalize;
			GLint byte_size;
			GLint count;
			GLenum type;
		};

	private:
		
		std::vector<AttributeDescriptor> m_AttributeDescriptors;

		GLint to_count(PackingType packing);

		GLenum to_opengl_type(DataType type);

		GLint get_byte_size(GLenum opengl_type, GLint count);

	public:

		std::vector<AttributeDescriptor> get_attribute_descriptors() const;

		void add_attribute(DataType type, PackingType packing, bool should_normalise = false);

		void add_attribute(bool attribute, bool should_normalise = false);
		
		void add_attribute(uint8_t attribute, bool should_normalise = false);
		
		void add_attribute(int8_t attribute, bool should_normalise = false);

		void add_attribute(uint16_t attribute, bool should_normalise = false);
		
		void add_attribute(int16_t attribute, bool should_normalise = false);

		void add_attribute(uint32_t attribute, bool should_normalise = false);
		
		void add_attribute(int32_t attribute, bool should_normalise = false);

		void add_attribute(float attribute, bool should_normalise = false);
		
		void add_attribute(double attribute, bool should_normalise = false);

		void add_attribute(glm::vec2 attribute, bool should_normalise = false);
		
		void add_attribute(glm::vec3 attribute, bool should_normalise = false);
		
		void add_attribute(glm::vec4 attribute, bool should_normalise = false);
		
		void add_attribute(glm::bvec2 attribute, bool should_normalise = false);
		
		void add_attribute(glm::bvec3 attribute, bool should_normalise = false);
		
		void add_attribute(glm::bvec4 attribute, bool should_normalise = false);
		
		void add_attribute(glm::uvec2 attribute, bool should_normalise = false);
		
		void add_attribute(glm::uvec3 attribute, bool should_normalise = false);
		
		void add_attribute(glm::uvec4 attribute, bool should_normalise = false);
		
		void add_attribute(glm::u8vec2 attribute, bool should_normalise = false);
		
		void add_attribute(glm::u8vec3 attribute, bool should_normalise = false);
		
		void add_attribute(glm::u8vec4 attribute, bool should_normalise = false);
		
		void add_attribute(glm::u16vec2 attribute, bool should_normalise = false);
		
		void add_attribute(glm::u16vec3 attribute, bool should_normalise = false);
		
		void add_attribute(glm::u16vec4 attribute, bool should_normalise = false);
		
		void add_attribute(glm::ivec2 attribute, bool should_normalise = false);
		
		void add_attribute(glm::ivec3 attribute, bool should_normalise = false);
		
		void add_attribute(glm::ivec4 attribute, bool should_normalise = false);
		
		void add_attribute(glm::i8vec2 attribute, bool should_normalise = false);
		
		void add_attribute(glm::i8vec3 attribute, bool should_normalise = false);
		
		void add_attribute(glm::i8vec4 attribute, bool should_normalise = false);
		
		void add_attribute(glm::i16vec2 attribute, bool should_normalise = false);
		
		void add_attribute(glm::i16vec3 attribute, bool should_normalise = false);
		
		void add_attribute(glm::i16vec4 attribute, bool should_normalise = false);
	};

}