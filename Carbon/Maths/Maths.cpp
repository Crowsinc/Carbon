#include "Maths.hpp"

#include <iostream>

#include <algorithm>

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

	std::tuple<glm::vec2, glm::vec2> find_rectangle_aabb(const glm::vec2& ul, const glm::vec2& ll, const glm::vec2& lr, const glm::vec2& ur)
	{
		// ul, ll, lr and ur a rectangle whose direction is the normal between ul and ur.
		// Where ul, lr, lr and ur are the vertices in the upper left, lower left, lower right
		// and upper right of the rectangle when it is facing straight north. 

		// Since the vertices describe a rectangle, we can immediately determine which 
		// vertices will be the furthest left, right, up and down, based on direction
		// of the rectangle and the relative locations of each vertex. This function will 
		// return a tuple containing the top left and bottom right coords of the aabb

		const glm::vec2 centre = (ul + lr) / 2.0f;
		const glm::vec2 top_centre = (ul + ur) / 2.0f;
		const glm::vec2 direction_vector = top_centre - centre;

		// Find the direction angle of the rectangle, where 0 radians is true North. 
		const float direction = atan2f(direction_vector.x, direction_vector.y);

		// If the rectangle's direction is between 0 and 90 degrees, then the rectangle
		// is pointing somewhere between North and East. Hence, ul will be the topmost
		// point, ur will be the rightmost, lr will be the bottommost, and ll the leftmost. 
		if(direction >= 0 && direction < PI_2)
		{
			// We return the AABB as top left and bottom right coordinates
			return std::make_tuple(glm::vec2{ll.x, ul.y}, glm::vec2{ur.x, lr.y});
		}
		// If the rectangle's direction is between 90 and 180 degrees, then the rectangle is 
		// pointing somewhere between South and East. Hence, ll will be the topmost point
		// ul the rightmost, ur the bottommost and lr the leftmost. 
		else if(direction >= PI_2 && direction < PI)
		{
			return std::make_tuple(glm::vec2{lr.x, ll.y}, glm::vec2{ul.x, ur.y});
		}
		// If the rectangle's direction is between 180 and 270 degrees, then the rectangle is 
		// pointing somewhere between South and West. Hence, lr will be the topmost point
		// ll the rightmost, ul the bottommost and ur the leftmost. 
		else if(direction >= -PI_2 && direction < 0)
		{
			return std::make_tuple(glm::vec2{ur.x, lr.y}, glm::vec2{ll.x, ul.y});
		}
		// If the rectangle's direction is between 270 and 360 degrees, then the rectangle is 
		// pointing somewhere between North and West. Hence, ur will be the topmost point
		// lr the rightmost, ll the bottommost and ul the leftmost. 
		else
		{
			return std::make_tuple(glm::vec2{ul.x, ur.y}, glm::vec2{lr.x, ll.y});
		}
	}
	
	//-------------------------------------------------------------------------------------

	std::tuple<glm::vec2, glm::vec2> find_rectangle_circle_bb(const glm::vec2& ul, const glm::vec2& ll, const glm::vec2& lr, const glm::vec2& ur)
	{
		// ul, ll, lr and ur a rectangle whose direction is the normal between ul and ur.
		// Where ul, lr, lr and ur are the vertices in the upper left, lower left, lower right
		// and upper right of the rectangle when it is facing straight north

		// Here we will find a rough bounding box of the rectangle by treating it as a circle
		// where its diameter is the length of the diagonals of the rectangle. 

		const float radius = glm::length(ul - lr) / 2.0f;

		const glm::vec2 centre = {(ul.x + ll.x + lr.x + ur.x) / 4.0f, (ul.y + ll.y + lr.y + ur.y) / 4.0f};

		return std::make_tuple(centre - radius, centre + radius);
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