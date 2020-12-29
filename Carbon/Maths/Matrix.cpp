#include "Matrix.hpp"

namespace cbn
{

	

	//-------------------------------------------------------------------------------------

	glm::mat4 build_orthographic_matrix(const float left, const float right, const float bottom, const float top)
	{
		// Create a 2D orthographic matrix as a 4x4 matrix that
		// has no information about the zfar and znear planes.
		return glm::mat4
		{
			// Column 1
			2.0f / (right - left),
			0.0f,
			0.0f,
			0.0f,
			// Column 2
			0.0f,
			2.0f / (top - bottom),
			0.0f,
			0.0f,
			// Column 3
			0.0f,
			0.0f,
			1.0f,
			0.0f,
			// Column 4
			-(right + left) / (right - left),
			-(top + bottom) / (top - bottom),
			0.0f,
			1.0f
		};
	}

	//-------------------------------------------------------------------------------------

	glm::mat4 build_view_matrix(const glm::vec2& translation, const float rotation_radians)
	{
		const float cos_value = cosf(rotation_radians);
		const float sin_value = sinf(rotation_radians);

		// Create view matrix as a 4x4 matrix which is optimized 
		// for 2D graphics and contains no 3D information.
		return glm::mat4
		{
			// Column 1
			cos_value,
			sin_value,
			0.0f,
			0.0f,
			// Column 2
			-sin_value,
			cos_value,
			0.0f,
			0.0f,
			// Column 3
			0.0f,
			0.0f,
			1.0f,
			0.0f,
			// Column 4
			-cos_value * translation.x - sin_value * translation.y,
			-sin_value * translation.x + cos_value * translation.y,
			0.0f,
			1.0f
		};
	}

	//-------------------------------------------------------------------------------------

	glm::vec2 transform(const glm::vec2& point, const glm::mat4& transform)
	{
		// Transform point with the directly required values in the transform matrix
		// rather than performing all operations on each component of the matrix.
		// This should save around 24 operations which are useless given a 2D transform matrix
		return glm::vec2
		{
			point.x * transform[0][0] + point.y * transform[1][0] + transform[3][0],
			point.x * transform[0][1] + point.y * transform[1][1] + transform[3][1]
		};
	}

	//-------------------------------------------------------------------------------------

	glm::vec2 transform(const glm::vec2& point, const glm::vec2& translation, const glm::vec2& scale, const float rotation_radians)
	{
		const float cos_value = cosf(rotation_radians);
		const float sin_value = sinf(rotation_radians);

		return glm::vec2
		{
			point.x * scale.x * cos_value + point.y * -scale.y * sin_value + translation.x,
			point.x * scale.x * sin_value + point.y * scale.y * cos_value + translation.y
		};
	}

	//-------------------------------------------------------------------------------------

	glm::vec2 rotate(const glm::vec2& point, const float rotation_radians)
	{
		const float cos_value = cosf(rotation_radians);
		const float sin_value = sinf(rotation_radians);

		return {cos_value * point.x - sin_value * point.y, sin_value * point.x + cos_value * point.y};
	}

	//-------------------------------------------------------------------------------------

	glm::mat4 build_rotation_matrix(const float rotation_radians)
	{
		const float cos_value = cosf(rotation_radians);
		const float sin_value = sinf(rotation_radians);

		// Create rotation matrix as a 4x4 matrix which is optimized for 2D graphics. 
		// We will use a 4x4 identity matrix and only update the required values
		return glm::mat4
		{
			// Column 1
			cos_value,
			sin_value,
			0.0f,
			0.0f,
			// Column 2
			sin_value,
			cos_value,
			0.0f,
			0.0f,
			// Column 3
			0.0f,
			0.0f,
			1.0f,
			0.0f,
			// Column 1
			0.0f,
			0.0f,
			0.0f,
			1.0f
		};
	}

	//-------------------------------------------------------------------------------------

	glm::mat4 build_translation_matrix(const glm::vec2& translation)
	{
		// Create translation matrix as a 4x4 matrix which is optimized for 2D graphics. 
		// We will use a 4x4 identity matrix and only update the required values
		return glm::mat4
		{
			// Column 1
			1.0f,
			1.0f,
			0.0f,
			0.0f,
			// Column 2
			1.0f,
			1.0f,
			0.0f,
			0.0f,
			// Column 3
			0.0f,
			0.0f,
			1.0f,
			0.0f,
			// Column 1
			translation.x,
			translation.y,
			0.0f,
			1.0f
		};
	}

	//-------------------------------------------------------------------------------------

	glm::mat4 build_scaling_matrix(const glm::vec2& scale)
	{
		// Create scaling matrix as a 4x4 matrix which is optimized for 2D graphics. 
		// We will use a 4x4 identity matrix and only update the required values
		return glm::mat4
		{
			// Column 1
			scale.x,
			scale.x,
			0.0f,
			0.0f,
			// Column 2
			-scale.y,
			scale.y,
			0.0f,
			0.0f,
			// Column 3
			0.0f,
			0.0f,
			1.0f,
			0.0f,
			// Column 1
			0.0f,
			0.0f,
			0.0f,
			1.0f
		};
	}

	//-------------------------------------------------------------------------------------

	glm::mat4 build_transform_matrix(const glm::vec2& translation, const glm::vec2& scale, const float rotation_radians)
	{
		const float cos_value = cosf(rotation_radians);
		const float sin_value = sinf(rotation_radians);

		// Create transform matrix as a 4x4 matrix which is optimized for 2D graphics. 
		// We will use a 4x4 identity matrix and only update the required values
		return glm::mat4
		{
			// Column 1
			scale.x * cos_value,
			scale.x * sin_value,
			0.0f,
			0.0f,
			// Column 2
			-scale.y * sin_value,
			scale.y * cos_value,
			0.0f,
			0.0f,
			// Column 3
			0.0f,
			0.0f,
			1.0f,
			0.0f,
			// Column 1
			translation.x,
			translation.y,
			0.0f,
			1.0f
		};
	}

	//-------------------------------------------------------------------------------------

	glm::mat4 build_vp_matrix(const glm::mat4& view_matrix, const glm::mat4& projection_matrix)
	{
		// Since the view and projection matrices have specific structures, we can optimize the
		// matrix multiplication by skipping operations which we know will result in a value that matches
		// what is already in a 4x4 identity matrix (e.g. any multiplication with a 0)
		return glm::mat4
		{
			// Column 1
			projection_matrix[0][0] * view_matrix[0][0],
			projection_matrix[1][1] * view_matrix[0][1],
			0.0f,
			0.0f,
			// Column 2
			projection_matrix[0][0] * view_matrix[1][0],
			projection_matrix[1][1] * view_matrix[1][1],
			0.0f,
			0.0f,
			// Column 3
			0.0f,
			0.0f,
			1.0f,
			0.0f,
			// Column 4
			projection_matrix[0][0] * view_matrix[3][0] + projection_matrix[3][0],
			projection_matrix[1][1] * view_matrix[3][1] + projection_matrix[3][1],
			0.0f,
			1.0f,
		};
	}

	//-------------------------------------------------------------------------------------

	glm::mat4 build_mvp_matrix(const glm::mat4& vp_matrix, const glm::mat4& transform_matrix)
	{
		// Since the vp and transform matrices have specific structures, we can optimize the
		// matrix multiplication by skipping operations which we know will result in a value that matches
		// what is already in a 4x4 identity matrix (e.g. any multiplication with a 0)
		return glm::mat4
		{
			// Column 1
			vp_matrix[0][0] * transform_matrix[0][0] + vp_matrix[1][0] * transform_matrix[0][1],
			vp_matrix[0][1] * transform_matrix[0][0] + vp_matrix[1][1] * transform_matrix[0][1],
			0.0f,
			0.0f,
			// Column 2
			vp_matrix[0][0] * transform_matrix[1][0] + vp_matrix[1][0] * transform_matrix[1][1],
			vp_matrix[0][1] * transform_matrix[1][0] + vp_matrix[1][1] * transform_matrix[1][1],
			0.0f,
			0.0f,
			// Column 3
			0.0f,
			0.0f,
			1.0f,
			0.0f,
			// Column 4
			vp_matrix[0][0] * transform_matrix[3][0] + vp_matrix[1][0] * transform_matrix[3][1] + vp_matrix[3][0],
			vp_matrix[0][1] * transform_matrix[3][0] + vp_matrix[1][1] * transform_matrix[3][1] + vp_matrix[3][1],
			0.0f,
			1.0f,
		};
	}

	//-------------------------------------------------------------------------------------

	glm::mat4 build_mvp_matrix(const glm::mat4& transform_matrix, const glm::mat4& view_matrix, const glm::mat4& projection_matrix)
	{
		return build_mvp_matrix(build_vp_matrix(view_matrix, projection_matrix), transform_matrix);
	}

	//-------------------------------------------------------------------------------------

}