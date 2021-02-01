#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <filesystem>

#include "../OpenGL/OpenGL.hpp"
#include "../../Memory/Resource.hpp"
#include "../../Data/Identity/Identifier.hpp"

namespace cbn
{

	class Shader
	{
		friend class ShaderProgram;
	public:

		enum class Stage : GLenum
		{
			VERTEX = GL_VERTEX_SHADER,
			FRAGMENT = GL_FRAGMENT_SHADER,
			GEOMETRY = GL_GEOMETRY_SHADER,
		};

		static std::tuple<SRes<Shader>, String> Open(const std::filesystem::path& shader_path, const Stage pipeline_stage);
		
		static std::tuple<SRes<Shader>, String> Compile(const std::string_view& shader_source, const Stage pipeline_stage);

	private:

		const GLuint m_ShaderID;
		const Stage m_PipelineStage;
		std::vector<Identifier> m_Uniforms;

		void find_uniform_names(const std::string_view& shader_source);

		explicit Shader(const Stage pipeline_stage);

	public:

		~Shader();

		Stage get_pipeline_stage() const;

		const std::vector<Identifier>& get_uniforms() const;

	};


}

