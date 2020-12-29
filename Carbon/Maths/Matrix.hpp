#pragma once

#include <glm/glm.hpp>

//TODO: rename to fast math or fast linear or something and have it be a mixture of mine and glm fast matrix/vector/float methods. 

namespace cbn
{
	glm::vec2 transform(const glm::vec2& point, const glm::mat4& transform);

	glm::vec2 transform(const glm::vec2& point, const glm::vec2& translation, const glm::vec2& scale, const float rotation_radians);
	
	glm::vec2 rotate(const glm::vec2& point, const float rotation_radians);
	
	glm::mat4 build_rotation_matrix(const float rotation_radians);
	
	glm::mat4 build_translation_matrix(const glm::vec2& translation);
	
	glm::mat4 build_scaling_matrix(const glm::vec2& scale);
	
	glm::mat4 build_transform_matrix(const glm::vec2& translation, const glm::vec2& scale, const float rotation_radians);
	
	glm::mat4 build_view_matrix(const glm::vec2& translation, const float rotation_radians);
	
	glm::mat4 build_orthographic_matrix(const float left, const float right, const float bottom, const float top);
	
	glm::mat4 build_mvp_matrix(const glm::mat4& transform_matrix, const glm::mat4& view_matrix, const glm::mat4& projection_matrix);
	
	glm::mat4 build_mvp_matrix(const glm::mat4& vp_matrix, const glm::mat4& transform_matrix);
	
	glm::mat4 build_vp_matrix(const glm::mat4& view_matrix, const glm::mat4& projection_matrix);

}