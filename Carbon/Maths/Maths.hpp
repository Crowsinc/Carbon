#pragma once

#include <math.h>
#include <tuple>
#include <glm/glm.hpp>

namespace cbn
{
	constexpr float PI = 3.14159265358979323846f;
	constexpr float PI_2 = PI / 2.0f;
	constexpr float PI_4 = PI / 4.0f;
	constexpr float PI_6 = PI / 6.0f;

	float to_degrees(const float radians);

	float to_radians(const float degrees);

	float lerp(const float start, const float end, const float t);
	
	glm::vec2 lerp(const glm::vec2& start, const glm::vec2& end, const float t);

	std::tuple<glm::vec2, glm::vec2> find_rectangle_aabb(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4);

	std::tuple<glm::vec2, glm::vec2> find_rectangle_circle_bb(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4);

	glm::vec2 transform(const glm::vec2& point, const glm::mat4& transform);

	glm::vec2 rotate(const glm::vec2& point, const float rotation_radians);

	glm::mat4 build_view_matrix(const glm::vec2& translation, const float rotation_radians);

	glm::mat4 build_orthographic_matrix(const float left, const float right, const float bottom, const float top);

	glm::mat4 build_transform_matrix(const glm::vec2& translation, const glm::vec2& scale, const float rotation_radians);

	glm::mat4 build_mvp_matrix(const glm::mat4& transform_matrix, const glm::mat4& view_matrix, const glm::mat4& projection_matrix);

	glm::mat4 build_mvp_matrix(const glm::mat4& vp_matrix, const glm::mat4& transform_matrix);

	glm::mat4 build_vp_matrix(const glm::mat4& view_matrix, const glm::mat4& projection_matrix);

}
