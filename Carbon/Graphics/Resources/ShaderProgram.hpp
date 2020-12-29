#pragma once

#include <glm/glm.hpp>
#include <unordered_map>

#include "Shader.hpp"
#include "../../Memory/Resource.hpp"
#include "../../Data/Identity/Identifier.hpp"

namespace cbn
{

	class ShaderProgram 
	{
	public:

		static std::tuple<SRes<ShaderProgram>, String> Create(const SRes<Shader>& vertex_shader, const SRes<Shader>& geometry_shader, const SRes<Shader>& fragment_shader);

	private:

		static GLuint s_BoundProgramID;

		const GLuint m_ProgramID;
		std::unordered_map<Identifier, GLint> m_UniformLocations;

		void find_shader_uniform_locations(const SRes<Shader>& shader);

		explicit ShaderProgram();

	public:

		~ShaderProgram();

		void bind() const;

		void unbind() const;

		bool is_bound() const;

		bool has_uniform(const Identifier& uniform) const;

		void set_uniform(const Identifier& uniform, const GLfloat value) const;

		void set_uniform(const Identifier& uniform, const GLint value) const;

		void set_uniform(const Identifier& uniform, const GLuint value) const;

		void set_uniform(const Identifier& uniform, const GLfloat value_1, const GLfloat value_2) const;

		void set_uniform(const Identifier& uniform, const GLint value_1, const GLint value_2) const;

		void set_uniform(const Identifier& uniform, const GLuint value_1, const GLuint value_2) const;

		void set_uniform(const Identifier& uniform, const glm::vec2& value) const;

		void set_uniform(const Identifier& uniform, const glm::ivec2& value) const;

		void set_uniform(const Identifier& uniform, const glm::uvec2& value) const;

		void set_uniform(const Identifier& uniform, const GLfloat value_1, const GLfloat value_2, const GLfloat value_3) const;

		void set_uniform(const Identifier& uniform, const GLint value_1, const GLint value_2, const GLint value_3) const;

		void set_uniform(const Identifier& uniform, const GLuint value_1, const GLuint value_2, const GLuint value_3) const;

		void set_uniform(const Identifier& uniform, const glm::vec3& value) const;

		void set_uniform(const Identifier& uniform, const glm::ivec3& value) const;

		void set_uniform(const Identifier& uniform, const glm::uvec3& value) const;

		void set_uniform(const Identifier& uniform, const GLfloat value_1, const GLfloat value_2, const GLfloat value_3, const GLfloat value_4) const;

		void set_uniform(const Identifier& uniform, const GLint value_1, const GLint value_2, const GLint value_3, const GLint value_4) const;

		void set_uniform(const Identifier& uniform, const GLuint value_1, const GLuint value_2, const GLuint value_3, const GLuint value_4) const;

		void set_uniform(const Identifier& uniform, const glm::vec4& value) const;

		void set_uniform(const Identifier& uniform, const glm::ivec4& value) const;

		void set_uniform(const Identifier& uniform, const glm::uvec4& value) const;

		void set_uniform(const Identifier& uniform, const glm::mat2& value) const;

		void set_uniform(const Identifier& uniform, const glm::mat3& value) const;

		void set_uniform(const Identifier& uniform, const glm::mat4& value) const;

		void set_uniform(const Identifier& uniform, const glm::mat2x3& value) const;

		void set_uniform(const Identifier& uniform, const glm::mat3x2& value) const;

		void set_uniform(const Identifier& uniform, const glm::mat2x4& value) const;

		void set_uniform(const Identifier& uniform, const glm::mat4x2& value) const;

		void set_uniform(const Identifier& uniform, const glm::mat3x4& value) const;

		void set_uniform(const Identifier& uniform, const glm::mat4x3& value) const;

	};

}