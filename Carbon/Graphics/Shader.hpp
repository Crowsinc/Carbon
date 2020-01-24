#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include "OpenGL.hpp"
#include "../Utility/Type.h"

namespace cbn
{

	class Shader
	{
		friend class ShaderProgram;
	public:

		enum class Stage
		{
			VERTEX = GL_VERTEX_SHADER,
			FRAGMENT = GL_FRAGMENT_SHADER,
			GEOMETRY = GL_GEOMETRY_SHADER,
		};

		static Ptr<Shader> Create(const std::string_view& shader_source, const Stage pipeline_stage, std::string& error_log);

	private:

		GLuint m_ShaderID;
		const Stage m_PipelineStage;
		std::vector<std::string> m_UniformNames;

		void find_uniform_names(const std::string_view& shader_source);

		Shader(const Stage pipeline_stage);

	public:

		~Shader();

		Stage get_pipeline_stage() const;

		const std::vector<std::string>& get_uniform_names() const;

	};


}

