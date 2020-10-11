#include "Shader.hpp"

#include <sstream>
#include <fstream>

#include "../../Utility/String.hpp"

namespace cbn
{
	
	//-------------------------------------------------------------------------------------

	SRes<Shader> Shader::Open(const std::filesystem::path& shader_path, const Stage pipeline_stage, std::string& error_log)
	{
		std::ifstream file_stream;

		file_stream.open(shader_path);

		if(file_stream.good())
		{
			std::stringstream shader_source;
			shader_source << file_stream.rdbuf();

			return Shader::Compile(shader_source.str(), pipeline_stage, error_log);
		}
		return nullptr;
	}

	//-------------------------------------------------------------------------------------

	SRes<Shader> Shader::Compile(const std::string_view& shader_source, const Stage pipeline_stage, std::string& error_log)
	{
		// Create a shader of the given pipeline stage
		SRes<Shader> shader = Resource::WrapShared(new Shader(pipeline_stage));

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

	void Shader::find_uniform_names(const std::string_view& shader_source)
	{
		// Clear existing uniform names just in case the shader 
		// initialization process is changed in the future
		m_UniformNames.clear();

		// Go through each line of the source one by one
		std::istringstream source_stream(shader_source.data());
		for(std::string stdline; std::getline(source_stream, stdline);)
		{
			String line(std::move(stdline));

			// Check if the line starts with the uniform keyword (whitespace is ignored)
			if(line.starts_with("uniform ", true))
			{
				// Since this line starts with the uniform keyword, split it based on spaces
				// to get the words in the line. The uniform name should be the 3rd word as shown
				// Format:  uniform <type> <name>;
				// The name will also probably include a ';' so we need to remove any of those 
				// from the line before we perform the split. 
				line.remove_all(";");
				auto split_line = line.split(" ");
				if(split_line.size() >= 3)
				{
					// If the split line contains 3 words, then add the 3rd will be the name
					String name = split_line[2];

					// If the name contains square brackets, then its an array and we need to add separate uniforms for each index
					if(name.contains("[") && name.contains("]"))
					{
						const auto start_index = name.find("[");
						const auto end_index = name.find("]");
						CBN_Assert(start_index < end_index, "Invalid array uniform");

						const String name_only = name.extract(0, start_index);
						const int array_size = std::stoi(name.extract(start_index + 1, end_index - start_index - 1).as_array());
						for(int i = 0; i < array_size; i++)
						{
							const String ending = "[" + std::to_string(i) + "]";
							m_UniformNames.emplace_back(name_only + ending);
						}
					} else m_UniformNames.emplace_back(name);
				}
			}
		}
	}

	//-------------------------------------------------------------------------------------

	Shader::Shader(const Shader::Stage pipeline_stage)
		: m_ShaderID(glCreateShader(static_cast<GLenum>(pipeline_stage))),
		  m_PipelineStage(pipeline_stage) {}

	//-------------------------------------------------------------------------------------

	Shader::~Shader()
	{
		glDeleteShader(m_ShaderID);
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