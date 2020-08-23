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

	SRes<ShaderProgram> ShaderProgram::Create(const SRes<Shader>& vertex_shader, const SRes<Shader>& geometry_shader, const SRes<Shader>& fragment_shader, std::string& error_log)
	{
		// Make sure that at least vertex shader and fragment shader have been supplied 
		if(!vertex_shader || !fragment_shader)
		{
			error_log = "Shader missing";
			return nullptr;
		}

		// Make sure each shader has the correct pipeline stage
		if(vertex_shader->get_pipeline_stage() != Shader::Stage::VERTEX || fragment_shader->get_pipeline_stage() != Shader::Stage::FRAGMENT ||
			(geometry_shader && geometry_shader->get_pipeline_stage() != Shader::Stage::GEOMETRY))
		{
			error_log = "Incorrect shader pipeline stage";
			return nullptr;
		}

		// Create a shader program to generate its OpenGL object
		SRes<ShaderProgram> program = Resource::WrapShared(new ShaderProgram()); 

		// Attach all the shaders to the shader program
		glAttachShader(program->m_ObjectID, vertex_shader->m_ObjectID);
		glAttachShader(program->m_ObjectID, fragment_shader->m_ObjectID);

		// The geometry shader is optional so check if it is given
		if(geometry_shader)
		{
			glAttachShader(program->m_ObjectID, geometry_shader->m_ObjectID);
		}

		// Link and validate the program
		glLinkProgram(program->m_ObjectID);
		glValidateProgram(program->m_ObjectID);

		// Check if the program linked correctly
		GLint linked = 0;
		glGetProgramiv(program->m_ObjectID, GL_LINK_STATUS, &linked);
		if(linked == GL_FALSE)
		{
			// If the link failed, get the length of the linking log
			GLint log_length = 0;
			glGetProgramiv(program->m_ObjectID, GL_INFO_LOG_LENGTH, &log_length);

			// If the log length is 0, then a driver issue probably ocurred 
			// so set the error log to an unknown error and return nullptr.
			// The program will delete its self with the programs destructor
			if(log_length == 0)
			{
				error_log = "Unknown Error";
				return nullptr;
			}

			// Create a vector to hold the log and retrieved it from OpenGL
			std::vector<GLchar> linking_log(log_length);
			glGetProgramInfoLog(program->m_ObjectID, log_length, &log_length, linking_log.data());

			// Set the error log to the linking error log and return nullptr
			// The program will delete its self with the programs destructor
			error_log = std::string(linking_log.data());
			return nullptr;
		}

		// With the shaders linked to the program, detach them and
		// find all the uniform locations for the shader uniforms
		program->find_shader_uniform_locations(vertex_shader);
		program->find_shader_uniform_locations(fragment_shader);

		glDetachShader(program->m_ObjectID, vertex_shader->m_ObjectID);
		glDetachShader(program->m_ObjectID, fragment_shader->m_ObjectID);

		// The geometry shader is optional so check if it is given
		if(geometry_shader)
		{
			program->find_shader_uniform_locations(geometry_shader);
			glDetachShader(program->m_ObjectID, geometry_shader->m_ObjectID);
		}

		// If we get here, the shader program was created successfully, so return it
		return program;
	}
	
	//-------------------------------------------------------------------------------------

	void ShaderProgram::find_shader_uniform_locations(const SRes<Shader>& shader)
	{
		// Go through all the uniform names in the shader and get their locations
		auto uniform_names = shader->get_uniform_names();
		for(std::string& uniform_name : uniform_names)
		{
			// Only try to the uniform location if the uniform name doesn't already exist
			if(m_UniformLocations.count(uniform_name) == 0)
			{
				GLint location = glGetUniformLocation(m_ObjectID, uniform_name.c_str());

				// If the location is no -1, then it actually exists in the program so add its location
				// Note that a location may not exist if the variable is optimized out of the shader
				if(location != -1)
				{
					m_UniformLocations.emplace(uniform_name, location);
				}
			}
		}
	}
	

	//-------------------------------------------------------------------------------------

	ShaderProgram::ShaderProgram()
		: GLSLObject(glCreateProgram, glDeleteProgram, glUseProgram) {}
	
	//-------------------------------------------------------------------------------------

	bool ShaderProgram::has_uniform(const CKey<std::string>& uniform) const
	{
		return m_UniformLocations.count(uniform);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const GLfloat value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform1f(m_UniformLocations.at(uniform), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const GLint value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);


		// Set the uniform for the shader program
		glUniform1i(m_UniformLocations.at(uniform), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const GLuint value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform1ui(m_UniformLocations.at(uniform), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const GLfloat value_1, const GLfloat value_2) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform2f(m_UniformLocations.at(uniform), value_1, value_2);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const GLint value_1, const GLint value_2) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform2i(m_UniformLocations.at(uniform), value_1, value_2);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const GLuint value_1, const GLuint value_2) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform2ui(m_UniformLocations.at(uniform), value_1, value_2);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::vec2& value) const
	{
		set_uniform(uniform, value.x, value.y);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::ivec2& value) const
	{
		set_uniform(uniform, value.x, value.y);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::uvec2& value) const
	{
		set_uniform(uniform, value.x, value.y);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const GLfloat value_1, const GLfloat value_2, const GLfloat value_3) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform3f(m_UniformLocations.at(uniform), value_1, value_2, value_3);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const GLint value_1, const GLint value_2, const GLint value_3) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform3i(m_UniformLocations.at(uniform), value_1, value_2, value_3);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const GLuint value_1, const GLuint value_2, const GLuint value_3) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform3ui(m_UniformLocations.at(uniform), value_1, value_2, value_3);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::vec3& value) const
	{
		set_uniform(uniform, value.x, value.y, value.z);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::ivec3& value) const
	{
		set_uniform(uniform, value.x, value.y, value.z);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::uvec3& value) const
	{
		set_uniform(uniform, value.x, value.y, value.z);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const GLfloat value_1, const GLfloat value_2, const GLfloat value_3, const GLfloat value_4) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform4f(m_UniformLocations.at(uniform), value_1, value_2, value_3, value_4);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const GLint value_1, const GLint value_2, const GLint value_3, const GLint value_4) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform4i(m_UniformLocations.at(uniform), value_1, value_2, value_3, value_4);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const GLuint value_1, const GLuint value_2, const GLuint value_3, const GLuint value_4) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniform4ui(m_UniformLocations.at(uniform), value_1, value_2, value_3, value_4);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::vec4& value) const
	{
		set_uniform(uniform, value.x, value.y, value.z, value.w);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::ivec4& value) const
	{
		set_uniform(uniform, value.x, value.y, value.z, value.w);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::uvec4& value) const
	{
		set_uniform(uniform, value.x, value.y, value.z, value.w);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::mat2& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix2fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::mat3& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix3fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}


	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::mat4& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix4fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::mat2x3& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix2x3fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::mat3x2& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix3x2fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::mat2x4& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix2x4fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::mat4x2& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix4x2fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::mat3x4& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix3x4fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const CKey<std::string>& uniform, const glm::mat4x3& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		VALIDATE_UNIFORM_LOC(uniform);

		// Set the uniform for the shader program
		glUniformMatrix4x3fv(m_UniformLocations.at(uniform), 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

}