#include "Shader.hpp"

#include <sstream>

#include "../Utility/Algortihms.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	Res<Shader> Shader::Create(const std::string_view& shader_source, const Stage pipeline_stage, std::string& error_log)
	{
		// Create a shader of the given pipeline stage
		Res<Shader> shader = Res<Shader>::Wrap(Shader{pipeline_stage}, &Shader::destroy);

		// Attempt to compile the given source into the shader, 
		const GLchar* c_shader_source = shader_source.data();
		glShaderSource(shader->m_ShaderID, 1, &c_shader_source, nullptr);
		glCompileShader(shader->m_ShaderID);

		// Check if the compilation was successful
		GLint compiled = 0;
		glGetShaderiv(shader->m_ShaderID, GL_COMPILE_STATUS, &compiled);
		if(compiled == GL_FALSE)
		{
			// If compilation failed, get the length of the compilation error log
			GLint log_length = 0;
			glGetShaderiv(shader->m_ShaderID, GL_INFO_LOG_LENGTH, &log_length);

			// Create a vector to hold the log and retrieve it from OpenGL
			std::vector<GLchar> compilation_log(log_length);
			glGetShaderInfoLog(shader->m_ShaderID, log_length, &log_length, compilation_log.data());

			// Set the given error log to the compilation error log 
			// and return nullptr. The shader destructor will handle
			// the deletion of the shader
			error_log = std::string(compilation_log.data());
			return nullptr;
		}

		// If we are here, then the shader compiled successfully. 
		// Search for all uniforms in the source and take note of them so 
		// that they can be queried at a later time by the Shader Program. 
		shader->find_uniform_names(shader_source);

		// The shader was successfully created so return it
		return shader;
	}

	//-------------------------------------------------------------------------------------

	void Shader::destroy(Shader& shader)
	{
		// Delete the shader object to avoid memory leaks
		glDeleteShader(shader.m_ShaderID);
	}

	//-------------------------------------------------------------------------------------

	void Shader::find_uniform_names(const std::string_view& shader_source)
	{
		// Clear existing uniform names just in case the shader 
		// initialization process is changed in the future
		m_UniformNames.clear();

		// Go through each line of the source one by one
		std::istringstream source_stream(shader_source.data());
		for(std::string line; std::getline(source_stream, line);)
		{
			// Check if the line starts with the uniform keyword (whitespace is ignored)
			if(starts_with(line, "uniform ", true))
			{
				// Since this line starts with the uniform keyword, split it based on spaces
				// to get the words in the line. The uniform name should be the 3rd word as shown
				// Format:  uniform <type> <name>;
				// The name will also probably include a ';' so we need to remove any of those 
				// from the line before we perform the split. 
				auto split_line = split(remove_all(line, ";"), " ");
				if(split_line.size() >= 3)
				{
					// If the split line contains 3 words, then add the 3rd to the uniform list
					m_UniformNames.emplace_back(split_line[2]);
				}
			}
		}
	}

	//-------------------------------------------------------------------------------------

	Shader::Shader(const Shader::Stage pipeline_stage)
		: m_ShaderID(NULL),
		m_PipelineStage(pipeline_stage)
	{
		// Create the shader OpenGL object in the correct pipeline stage
		m_ShaderID = glCreateShader(static_cast<GLuint>(pipeline_stage));
	}

	//-------------------------------------------------------------------------------------

	Shader::Stage Shader::get_pipeline_stage() const
	{
		return m_PipelineStage;
	}
	
	//-------------------------------------------------------------------------------------

	const std::vector<std::string>& Shader::get_uniform_names() const
	{
		return m_UniformNames;
	}

	//-------------------------------------------------------------------------------------
}