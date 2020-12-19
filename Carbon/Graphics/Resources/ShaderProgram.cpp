#include "ShaderProgram.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../Diagnostics/Assert.hpp"


//-------------------------------------------------------------------------------------

#define VALIDATE_UNIFORM_LOC(uniform) \
CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound"); \
CBN_Assert(m_UniformLocations.count(uniform) == 1, "Cannot set uniform, as uniform location does not exist")

//-------------------------------------------------------------------------------------

namespace cbn
{
	//-------------------------------------------------------------------------------------

	GLuint ShaderProgram::s_BoundProgramID = 0;

	//-------------------------------------------------------------------------------------

	std::tuple<SRes<ShaderProgram>, String> ShaderProgram::Create(const SRes<Shader>& vertex_shader, const SRes<Shader>& geometry_shader, const SRes<Shader>& fragment_shader)
	{
		// Make sure that at least vertex shader and fragment shader have been supplied 
		if(!vertex_shader || !fragment_shader)
		{
			return {nullptr, "Shader missing"};
		}

		// Make sure each shader has the correct pipeline stage
		if(vertex_shader->get_pipeline_stage() != Shader::Stage::VERTEX || fragment_shader->get_pipeline_stage() != Shader::Stage::FRAGMENT ||
			(geometry_shader && geometry_shader->get_pipeline_stage() != Shader::Stage::GEOMETRY))
		{
			return {nullptr, "Incorrect shader pipeline stage"};
		}

		// Create a shader program to generate its OpenGL object
		SRes<ShaderProgram> program = Resource::WrapShared(new ShaderProgram()); 

		// Attach all the shaders to the shader program
		glAttachShader(program->m_ProgramID, vertex_shader->m_ShaderID);
		glAttachShader(program->m_ProgramID, fragment_shader->m_ShaderID);

		// The geometry shader is optional so check if it is given
		if(geometry_shader)
		{
			glAttachShader(program->m_ProgramID, geometry_shader->m_ShaderID);
		}

		// Link and validate the program
		glLinkProgram(program->m_ProgramID);
		glValidateProgram(program->m_ProgramID);

		// Check if the program linked correctly
		GLint linked = 0;
		glGetProgramiv(program->m_ProgramID, GL_LINK_STATUS, &linked);
		if(linked == GL_FALSE)
		{
			// If the link failed, get the length of the linking log
			GLint log_length = 0;
			glGetProgramiv(program->m_ProgramID, GL_INFO_LOG_LENGTH, &log_length);

			// If the log length is 0, then a driver issue probably ocurred 
			// so set the error log to an unknown error and return nullptr.
			// The program will delete its self with the programs destructor
			if(log_length == 0)
			{
				return {nullptr, "Unknown Error"};
			}

			// Create a vector to hold the log and retrieved it from OpenGL
			std::vector<GLchar> linking_log(log_length);
			glGetProgramInfoLog(program->m_ProgramID, log_length, &log_length, linking_log.data());

			// Set the error log to the linking error log and return nullptr
			// The program will delete its self with the programs destructor
			return {nullptr, linking_log.data()};
		}

		// With the shaders linked to the program, detach them and
		// find all the uniform locations for the shader uniforms
		program->find_shader_uniform_locations(vertex_shader);
		program->find_shader_uniform_locations(fragment_shader);

		glDetachShader(program->m_ProgramID, vertex_shader->m_ShaderID);
		glDetachShader(program->m_ProgramID, fragment_shader->m_ShaderID);

		// The geometry shader is optional so check if it is given
		if(geometry_shader)
		{
			program->find_shader_uniform_locations(geometry_shader);
			glDetachShader(program->m_ProgramID, geometry_shader->m_ShaderID);
		}

		// If we get here, the shader program was created successfully, so return it
		return {program, ""};
	}
	
	//-------------------------------------------------------------------------------------

	void ShaderProgram::find_shader_uniform_locations(const SRes<Shader>& shader)
	{
		// Go through all the uniform names in the shader and get their locations
		auto uniforms = shader->get_uniforms();
		for(Identifier& uniform : uniforms)
		{
			// Only try to the uniform location if the uniform name doesn't already exist
			if(m_UniformLocations.count(uniform) == 0)
			{
				GLint location = glGetUniformLocation(m_ProgramID, uniform.alias().as_array());

				// If the location is no -1, then it actually exists in the program so add its location
				// Note that a location may not exist if the variable is optimized out of the shader
				if(location != -1)
				{
					m_UniformLocations.emplace(uniform, location);
				}
			}
		}
	}
	

	//-------------------------------------------------------------------------------------

	ShaderProgram::ShaderProgram()
		: m_ProgramID(glCreateProgram()) {} 

	//-------------------------------------------------------------------------------------

	ShaderProgram::~ShaderProgram()
	{
		glDeleteProgram(m_ProgramID);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::bind() const
	{
		if(!is_bound())
		{
			s_BoundProgramID = m_ProgramID;
			glUseProgram(m_ProgramID);
		}
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::unbind() const
	{
		if(is_bound())
		{
			s_BoundProgramID = 0;
			glUseProgram(0);
		}
	}

	//-------------------------------------------------------------------------------------

	bool ShaderProgram::is_bound() const
	{
		return s_BoundProgramID == m_ProgramID;
	}
	
	//-------------------------------------------------------------------------------------

	bool ShaderProgram::has_uniform(const Identifier& uniform) const
	{
		return m_UniformLocations.count(uniform);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const GLfloat value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform1f(m_UniformLocations.at(uniform), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const GLint value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);


		// Set the uniform for the shader program
		glUniform1i(m_UniformLocations.at(uniform), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const GLuint value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform1ui(m_UniformLocations.at(uniform), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const GLfloat value_1, const GLfloat value_2) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform2f(m_UniformLocations.at(uniform), value_1, value_2);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const GLint value_1, const GLint value_2) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform2i(m_UniformLocations.at(uniform), value_1, value_2);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const GLuint value_1, const GLuint value_2) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform2ui(m_UniformLocations.at(uniform), value_1, value_2);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::vec2& value) const
	{
		set_uniform(uniform, value.x, value.y);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::ivec2& value) const
	{
		set_uniform(uniform, value.x, value.y);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::uvec2& value) const
	{
		set_uniform(uniform, value.x, value.y);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const GLfloat value_1, const GLfloat value_2, const GLfloat value_3) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform3f(m_UniformLocations.at(uniform), value_1, value_2, value_3);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const GLint value_1, const GLint value_2, const GLint value_3) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform3i(m_UniformLocations.at(uniform), value_1, value_2, value_3);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const GLuint value_1, const GLuint value_2, const GLuint value_3) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform3ui(m_UniformLocations.at(uniform), value_1, value_2, value_3);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::vec3& value) const
	{
		set_uniform(uniform, value.x, value.y, value.z);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::ivec3& value) const
	{
		set_uniform(uniform, value.x, value.y, value.z);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::uvec3& value) const
	{
		set_uniform(uniform, value.x, value.y, value.z);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const GLfloat value_1, const GLfloat value_2, const GLfloat value_3, const GLfloat value_4) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform4f(m_UniformLocations.at(uniform), value_1, value_2, value_3, value_4);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const GLint value_1, const GLint value_2, const GLint value_3, const GLint value_4) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform4i(m_UniformLocations.at(uniform), value_1, value_2, value_3, value_4);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const GLuint value_1, const GLuint value_2, const GLuint value_3, const GLuint value_4) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform4ui(m_UniformLocations.at(uniform), value_1, value_2, value_3, value_4);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::vec4& value) const
	{
		set_uniform(uniform, value.x, value.y, value.z, value.w);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::ivec4& value) const
	{
		set_uniform(uniform, value.x, value.y, value.z, value.w);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::uvec4& value) const
	{
		set_uniform(uniform, value.x, value.y, value.z, value.w);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::mat2& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix2fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::mat3& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix3fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}


	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::mat4& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix4fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::mat2x3& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix2x3fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::mat3x2& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix3x2fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::mat2x4& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix2x4fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::mat4x2& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix4x2fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::mat3x4& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix3x4fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const Identifier& uniform, const glm::mat4x3& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix4x3fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

}