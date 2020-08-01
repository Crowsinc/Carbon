#include "ShaderProgram.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../Diagnostics/Assert.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	GLuint ShaderProgram::s_BoundProgramID = 0;

	//-------------------------------------------------------------------------------------

	Res<ShaderProgram> ShaderProgram::Create(const Res<Shader>& vertex_shader, const Res<Shader>& geometry_shader, const Res<Shader>& fragment_shader, std::string& error_log)
	{
		// Make sure that at least vertex shader and fragment shader have been supplied 
		if(!vertex_shader.exists() || !fragment_shader.exists())
		{
			error_log = "Shader missing";
			return nullptr;
		}

		// Make sure each shader has the correct pipeline stage
		if(vertex_shader->get_pipeline_stage() != Shader::Stage::VERTEX || fragment_shader->get_pipeline_stage() != Shader::Stage::FRAGMENT ||
			(geometry_shader.exists() && geometry_shader->get_pipeline_stage() != Shader::Stage::GEOMETRY))
		{
			error_log = "Incorrect shader pipeline stage";
			return nullptr;
		}

		// Create a shader program to generate its OpenGL object
		ShaderProgram program_resource{};
		Res<ShaderProgram> program = Res<ShaderProgram>::Wrap(program_resource,&ShaderProgram::destroy);

		// Attach all the shaders to the shader program
		glAttachShader(program->m_ProgramID, vertex_shader->m_ShaderID);
		glAttachShader(program->m_ProgramID, fragment_shader->m_ShaderID);

		// The geometry shader is optional so check if it is given
		if(geometry_shader.exists())
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
				error_log = "Unknown Error";
				return nullptr;
			}

			// Create a vector to hold the log and retrieved it from OpenGL
			std::vector<GLchar> linking_log(log_length);
			glGetProgramInfoLog(program->m_ProgramID, log_length, &log_length, linking_log.data());

			// Set the error log to the linking error log and return nullptr
			// The program will delete its self with the programs destructor
			error_log = std::string(linking_log.data());
			return nullptr;
		}

		// With the shaders linked to the program, detach them and
		// find all the uniform locations for the shader uniforms
		program->find_shader_uniform_locations(vertex_shader);
		program->find_shader_uniform_locations(fragment_shader);

		glDetachShader(program->m_ProgramID, vertex_shader->m_ShaderID);
		glDetachShader(program->m_ProgramID, fragment_shader->m_ShaderID);

		// The geometry shader is optional so check if it is given
		if(geometry_shader.exists())
		{
			program->find_shader_uniform_locations(geometry_shader);
			glDetachShader(program->m_ProgramID, geometry_shader->m_ShaderID);
		}

		// If we get here, the shader program was created successfully, so return it
		return program;
	}
	
	//-------------------------------------------------------------------------------------

	void ShaderProgram::find_shader_uniform_locations(const Shader& shader)
	{
		// Go through all the uniform names in the shader and get their locations
		auto uniform_names = shader.get_uniform_names();
		for(std::string& uniform_name : uniform_names)
		{
			// Only try to the uniform location if the uniform name doesn't already exist
			if(m_UniformLocations.count(uniform_name) == 0)
			{
				GLint location = glGetUniformLocation(m_ProgramID, uniform_name.c_str());

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

	void ShaderProgram::destroy(ShaderProgram& shader_program)
	{
		glDeleteProgram(shader_program.m_ProgramID);
	}

	//-------------------------------------------------------------------------------------

	ShaderProgram::ShaderProgram()
		: m_ProgramID(NULL)
	{
		// Create the shader program 
		m_ProgramID = glCreateProgram();
	}
	
	//-------------------------------------------------------------------------------------
	
	void ShaderProgram::bind()
	{
		// Check if the shader program is already bound, if it is then don't bind it 
		if(!is_bound())
		{
			// Bind the shader and update the bound program ID
			glUseProgram(m_ProgramID);
			s_BoundProgramID = m_ProgramID;
		}
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::unbind()
	{
		// Check if this shader program is actually bound before trying to unbind it
		if(is_bound())
		{
			// Unbind the current progam and update the bound program ID to 0
			glUseProgram(0);
			s_BoundProgramID = 0;
		}
	}

	//-------------------------------------------------------------------------------------

	bool ShaderProgram::is_bound() const
	{
		return s_BoundProgramID == m_ProgramID;
	}

	//-------------------------------------------------------------------------------------

	bool ShaderProgram::has_uniform(const std::string& uniform_name) const
	{
		return m_UniformLocations.count(uniform_name);
	}

	//-------------------------------------------------------------------------------------

	GLint ShaderProgram::get_uniform_location(const std::string& uniform_name) const
	{
		CBN_Assert(has_uniform(uniform_name), "Cannot get location to uniform which does not exist");
		return m_UniformLocations.at(uniform_name);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const GLfloat value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniform1f(uniform_location, value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const GLint value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniform1i(uniform_location, value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const GLuint value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniform1ui(uniform_location, value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const GLfloat value_1, const GLfloat value_2) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniform2f(uniform_location, value_1, value_2);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const GLint value_1, const GLint value_2) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniform2i(uniform_location, value_1, value_2);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const GLuint value_1, const GLuint value_2) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniform2ui(uniform_location, value_1, value_2);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::vec2& value) const
	{
		set_uniform(uniform_location, value.x, value.y);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::ivec2& value) const
	{
		set_uniform(uniform_location, value.x, value.y);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::uvec2& value) const
	{
		set_uniform(uniform_location, value.x, value.y);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const GLfloat value_1, const GLfloat value_2, const GLfloat value_3) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniform3f(uniform_location, value_1, value_2, value_3);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const GLint value_1, const GLint value_2, const GLint value_3) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniform3i(uniform_location, value_1, value_2, value_3);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const GLuint value_1, const GLuint value_2, const GLuint value_3) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniform3ui(uniform_location, value_1, value_2, value_3);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::vec3& value) const
	{
		set_uniform(uniform_location, value.x, value.y, value.z);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::ivec3& value) const
	{
		set_uniform(uniform_location, value.x, value.y, value.z);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::uvec3& value) const
	{
		set_uniform(uniform_location, value.x, value.y, value.z);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const GLfloat value_1, const GLfloat value_2, const GLfloat value_3, const GLfloat value_4) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniform4f(uniform_location, value_1, value_2, value_3, value_4);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const GLint value_1, const GLint value_2, const GLint value_3, const GLint value_4) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniform4i(uniform_location, value_1, value_2, value_3, value_4);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const GLuint value_1, const GLuint value_2, const GLuint value_3, const GLuint value_4) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniform4ui(uniform_location, value_1, value_2, value_3, value_4);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::vec4& value) const
	{
		set_uniform(uniform_location, value.x, value.y, value.z, value.w);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::ivec4& value) const
	{
		set_uniform(uniform_location, value.x, value.y, value.z, value.w);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::uvec4& value) const
	{
		set_uniform(uniform_location, value.x, value.y, value.z, value.w);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::mat2& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniformMatrix2fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::mat3& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniformMatrix3fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
	}


	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::mat4& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::mat2x3& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniformMatrix2x3fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::mat3x2& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniformMatrix3x2fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::mat2x4& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniformMatrix2x4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::mat4x2& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniformMatrix4x2fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::mat3x4& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniformMatrix3x4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const GLint uniform_location, const glm::mat4x3& value) const
	{
		// Make sure that the program is bound and that the uniform location exists
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(std::any_of(m_UniformLocations.begin(), m_UniformLocations.end(), [&](std::pair<std::string, GLint> pair)
		{
			return uniform_location == pair.second;
		}), "Cannot set uniform, as uniform location does not exist");

		// Set the uniform for the shader program
		glUniformMatrix4x3fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const GLfloat value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const GLint value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const GLuint value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const GLfloat value_1, const GLfloat value_2) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value_1, value_2);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const GLint value_1, const GLint value_2) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value_1, value_2);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const GLuint value_1, const GLuint value_2) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value_1, value_2);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::vec2& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::ivec2& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::uvec2& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const GLfloat value_1, const GLfloat value_2, const GLfloat value_3) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value_1, value_2, value_3);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const GLint value_1, const GLint value_2, const GLint value_3) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value_1, value_2, value_3);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const GLuint value_1, const GLuint value_2, const GLuint value_3) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value_1, value_2, value_3);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::vec3& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::ivec3& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::uvec3& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const GLfloat value_1, const GLfloat value_2, const GLfloat value_3, const GLfloat value_4) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value_1, value_2, value_3, value_4);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const GLint value_1, const GLint value_2, const GLint value_3, const GLint value_4) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value_1, value_2, value_3, value_4);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const GLuint value_1, const GLuint value_2, const GLuint value_3, const GLuint value_4) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value_1, value_2, value_3, value_4);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::vec4& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::ivec4& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::uvec4& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::mat2& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::mat3& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::mat4& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::mat2x3& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::mat3x2& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::mat2x4& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::mat4x2& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::mat3x4& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------

	void ShaderProgram::set_uniform(const std::string& uniform_name, const glm::mat4x3& value) const
	{
		// Make sure that the program is bound and that the uniform name is valid
		CBN_Assert(is_bound(), "Cannot set uniform to a shader program which is not bound");
		CBN_Assert(has_uniform(uniform_name), "Cannot set uniform as it does not exist");

		// Call the uniform location version of the method
		set_uniform(get_uniform_location(uniform_name), value);
	}

	//-------------------------------------------------------------------------------------
}