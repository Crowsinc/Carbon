#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include "../OpenGL/OpenGL.hpp"
#include "../OpenGL/GLSLObject.hpp"
#include "../../Memory/Resource.hpp"

namespace cbn
{

	class Shader : private GLSLObject<GL_SHADER>
	{
		friend class ShaderProgram;
	public:

		enum class Stage
		{
			VERTEX = GL_VERTEX_SHADER,
			FRAGMENT = GL_FRAGMENT_SHADER,
			GEOMETRY = GL_GEOMETRY_SHADER,
		};

		static SRes<Shader> Compile(const std::string_view& shader_source, const Stage pipeline_stage, std::string& error_log);

	private:

		Stage m_PipelineStage;
		std::vector<std::string> m_UniformNames;

		void find_uniform_names(const std::string_view& shader_source);

		explicit Shader(const Stage pipeline_stage);

	public:

		Stage get_pipeline_stage() const;

		const std::vector<std::string>& get_uniform_names() const;

	};


}

