#include "Transform.hpp"

#include <glm/gtx/projection.hpp>

namespace cbn
{

	//-------------------------------------------------------------------------------------

	Transform::Transform()
		: Transform(0, 0, 0, 1, 1) {}
	
	//-------------------------------------------------------------------------------------

	Transform::Transform(const float x, const float y, const float rotation_degrees, const float scale_x, const float scale_y)
	{
		translate_to(x, y);
		rotate_to(rotation_degrees);
		scale_to(scale_x, scale_y);
	}

	//-------------------------------------------------------------------------------------

	Transform::Transform(const glm::vec2& translation, const float rotation_degrees, const glm::vec2& scale)
	{
		translate_to(translation);
		rotate_to(rotation_degrees);
		scale_to(scale);
	}

	//-------------------------------------------------------------------------------------

	glm::vec2 Transform::apply(const glm::vec2& point) const
	{
		return transform(point, build_transform_matrix(translation(), scale(), rotation_radians()));
	}
	
	//-------------------------------------------------------------------------------------

	Transform Transform::apply(const Transform& other) const
	{
		return {other.translation() + translation(), other.rotation_degrees() + rotation_degrees(), other.scale() * scale()};
	}
	
	//-------------------------------------------------------------------------------------

}