#pragma once

#include <cmath>
#include <glm/glm.hpp>

namespace cbn
{
	constexpr float PI = 3.14159265358979323846f;

	template<typename T>
	T to_degrees(const T radians);

	template<typename T>
	T to_radians(const T degrees);

	template<typename T>
	T lerp(const T start, const T end, const float t);

	// Render Matrix Functions

	glm::mat4 create_orthographic_2d(const float left, const float right, const float bottom, const float top);

	glm::mat4 create_transform_2d(const glm::vec2& translation, const glm::vec2& scale, const float rotation_radians);

	glm::mat4 create_view_2d(const glm::vec2& translation, const float rotation_radians);

	glm::vec2 fast_2d_transform(const glm::vec2& point, const glm::mat4& transform);

	glm::mat4 mvp_fast_multiply_2d(const glm::mat4& vp_matrix, const glm::mat4 transform_matrix);

	glm::mat4 vp_fast_multiply_2d(const glm::mat4& view_matrix, const glm::mat4 projection_matrix);

	glm::mat4 mvp_fast_multiply_2d(const glm::mat4& transform_matrix, const glm::mat4 view_matrix, const glm::mat4& projection_matrix);

}

// Template implementation
#include "Maths.tpp"