#include "Maths.hpp"


namespace cbn
{

	//-------------------------------------------------------------------------------------

	float to_degrees(const float radians)
	{
		return (radians * 180.0f) / PI;
	}

	//-------------------------------------------------------------------------------------


	float to_radians(const float degrees)
	{
		return (degrees * PI) / 180.0f;
	}

	//-------------------------------------------------------------------------------------


	float lerp(const float start, const float end, const float t)
	{
		return start + (end - start) * t;
	}

	//-------------------------------------------------------------------------------------

	glm::vec2 lerp(const glm::vec2& start, const glm::vec2& end, const float t)
	{
		// Wrie this in terms of the float lerp to re-use code
		return glm::vec2{lerp(start.x,end.x,t), lerp(start.y, end.y,t)};
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

	glm::vec2 rotate(const glm::vec2& point, const float rotation_radians)
	{
		const float cos_value = cosf(rotation_radians);
		const float sin_value = sinf(rotation_radians);

		return {cos_value * point.x - sin_value * point.y, sin_value * point.x + cos_value * point.y};
	}

	//-------------------------------------------------------------------------------------

	std::tuple<glm::vec2, glm::vec2> find_rectangle_aabb(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4)
	{
		// Since the vertices describe a rectangle, we can immediately determine which 
		// vertices will be the furthest left, right, up and down, based on direction
		// of the rectangle and the relative locations of each vertex. This function will 
		// return a tuple containing the top left and bottom right coords of the aabb

		// Treat the midpoint between the vertices v1 and v2, which should be adjacent,
		// as direction that the rectangle is facing, and determine the direction angle
		const float direction = atan2f((v1.x + v2.x) / 2.0f, (v1.y + v2.y) / 2.0f);

		// If the rectangle's direction is between 0 and 90 degrees, then v1 is the upmost
		// vertex, v3 is the leftmost, v4 is the bottommost and v2 is the rightmost
		if(direction >= 0 && direction < PI_2)
		{
			return std::make_tuple(glm::vec2{v3.x, v1.y}, glm::vec2{v2.x, v4.y});
		}
		// If the rectangle's direction is between 90 and 180 degrees, then v2 is the upmost
		// vertex, v1 is the leftmost, v3 is the bottommost and v4 is the rightmost. 
		else if(direction >= PI_2 && direction < PI)
		{
			return std::make_tuple(glm::vec2{v1.x, v2.y}, glm::vec2{v4.x, v3.y});
		}
		// If the rectangle's direction is between 0 and -90 degrees, then v3 is the upmost
		// vertex, v4 is the leftmost, v2 is the bottommost and v1 is the rightmost. 
		else if(direction >= -PI_2 && direction < 0)
		{
			return std::make_tuple(glm::vec2{v4.x, v3.y}, glm::vec2{v1.x, v2.y});
		}
		// If the rectangle's direction is between -90 and -180 degrees, then v4 is the upmost
		// vertex, v2 is the leftmost, v1 is the bottommost and v3 is the rightmost. 
		else
		{
			return std::make_tuple(glm::vec2{v2.x, v4.y}, glm::vec2{v3.x, v1.y});
		}
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