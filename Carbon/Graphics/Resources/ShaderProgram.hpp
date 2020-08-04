#pragma once

#include <glm/glm.hpp>
#include <unordered_map>

#include "Shader.hpp"
#include "../OpenGL/GLSLObject.hpp"
#include "../../Memory/Resource.hpp"

namespace cbn
{

	class ShaderProgram : public GLSLObject<GL_PROGRAM>
	{
	public:

		static SRes<ShaderProgram> Create(const SRes<Shader>& vertex_shader, const SRes<Shader>& geometry_shader, const SRes<Shader>& fragment_shader, std::string& error_log);

	private:

		std::unordered_map<std::string, GLint> m_UniformLocations;

		void find_shader_uniform_locations(const SRes<Shader>& shader);

		explicit ShaderProgram();

	public:

		bool has_uniform(const std::string& uniform_name) const;

		GLint get_uniform_location(const std::string& uniform_name) const;

		void set_uniform(const GLint uniform_location, const GLfloat value) const;

		void set_uniform(const GLint uniform_location, const GLint value) const;

		void set_uniform(const GLint uniform_location, const GLuint value) const;

		void set_uniform(const GLint uniform_location, const GLfloat value_1, const GLfloat value_2) const;

		void set_uniform(const GLint uniform_location, const GLint value_1, const GLint value_2) const;

		void set_uniform(const GLint uniform_location, const GLuint value_1, const GLuint value_2) const;

		void set_uniform(const GLint uniform_location, const glm::vec2& value) const;

		void set_uniform(const GLint uniform_location, const glm::ivec2& value) const;

		void set_uniform(const GLint uniform_location, const glm::uvec2& value) const;

		void set_uniform(const GLint uniform_location, const GLfloat value_1, const GLfloat value_2, const GLfloat value_3) const;

		void set_uniform(const GLint uniform_location, const GLint value_1, const GLint value_2, const GLint value_3) const;

		void set_uniform(const GLint uniform_location, const GLuint value_1, const GLuint value_2, const GLuint value_3) const;

		void set_uniform(const GLint uniform_location, const glm::vec3& value) const;

		void set_uniform(const GLint uniform_location, const glm::ivec3& value) const;

		void set_uniform(const GLint uniform_location, const glm::uvec3& value) const;

		void set_uniform(const GLint uniform_location, const GLfloat value_1, const GLfloat value_2, const GLfloat value_3, const GLfloat value_4) const;

		void set_uniform(const GLint uniform_location, const GLint value_1, const GLint value_2, const GLint value_3, const GLint value_4) const;

		void set_uniform(const GLint uniform_location, const GLuint value_1, const GLuint value_2, const GLuint value_3, const GLuint value_4) const;

		void set_uniform(const GLint uniform_location, const glm::vec4& value) const;

		void set_uniform(const GLint uniform_location, const glm::ivec4& value) const;

		void set_uniform(const GLint uniform_location, const glm::uvec4& value) const;

		void set_uniform(const GLint uniform_location, const glm::mat2& value) const;

		void set_uniform(const GLint uniform_location, const glm::mat3& value) const;

		void set_uniform(const GLint uniform_location, const glm::mat4& value) const;

		void set_uniform(const GLint uniform_location, const glm::mat2x3& value) const;

		void set_uniform(const GLint uniform_location, const glm::mat3x2& value) const;

		void set_uniform(const GLint uniform_location, const glm::mat2x4& value) const;

		void set_uniform(const GLint uniform_location, const glm::mat4x2& value) const;

		void set_uniform(const GLint uniform_location, const glm::mat3x4& value) const;

		void set_uniform(const GLint uniform_location, const glm::mat4x3& value) const;

	};

}