#include "Transform.hpp"

#include <glm/gtx/projection.hpp>

namespace cbn
{

	//-------------------------------------------------------------------------------------

	Transform::Transform(const float x, const float y, const float scale_x, const float scale_y, const float rotation_degrees)
		: m_Translation(glm::vec2(x, y)),
		m_Rotation(to_radians(rotation_degrees)),
		m_Scale(glm::vec2(scale_x, scale_y)),
		m_TransformCache(1.0),
		m_CacheOutdated(true)
	{}

	//-------------------------------------------------------------------------------------

	Transform::Transform(const glm::vec2& translation, const glm::vec2& scale, const float rotation_degrees)
		: m_Translation(translation),
		m_Rotation(to_radians(rotation_degrees)),
		m_Scale(scale),
		m_TransformCache(1.0),
		m_CacheOutdated(true)
	{}

	//-------------------------------------------------------------------------------------

	void Transform::translate_by(float x, float y)
	{
		// Flag that the cached transform needs to be be updated
		m_CacheOutdated = true;

		// Update the translation
		m_Translation.x += x;
		m_Translation.y += y;
	}

	//-------------------------------------------------------------------------------------

	void Transform::translate_by(const glm::vec2& translation)
	{
		// Flag that the cached transform needs to be be updated
		m_CacheOutdated = true;

		// Update the translation
		m_Translation += translation;
	}

	//-------------------------------------------------------------------------------------

	void Transform::translate_to(float x, float y)
	{
		// Flag that the cached transform needs to be be updated
		m_CacheOutdated = true;

		// Update the translation
		m_Translation.x = x;
		m_Translation.y = y;
	}

	//-------------------------------------------------------------------------------------

	void Transform::translate_to(const glm::vec2& position)
	{
		// Flag that the cached transform needs to be be updated
		m_CacheOutdated = true;

		// Update the translation
		m_Translation = position;
	}

	//-------------------------------------------------------------------------------------

	void Transform::rotate_by(float degrees)
	{
		// Flag that the cached transform needs to be be updated
		m_CacheOutdated = true;

		// Update the rotation, making sure to convert to radians
		m_Rotation += to_radians(degrees);
	}

	//-------------------------------------------------------------------------------------

	void Transform::rotate_to(float degrees)
	{
		// Flag that the cached transform needs to be be updated
		m_CacheOutdated = true;

		// Update the rotation, making sure to convert to radians
		m_Rotation = to_radians(degrees);
	}

	//-------------------------------------------------------------------------------------

	void Transform::scale_by(float scale)
	{
		// Flag that the cached transform needs to be be updated
		m_CacheOutdated = true;

		// Update the scaling
		m_Scale.x += scale;
		m_Scale.y += scale;
	}

	//-------------------------------------------------------------------------------------

	void Transform::scale_by(const glm::vec2& scaling)
	{
		// Flag that the cached transform needs to be be updated
		m_CacheOutdated = true;

		// Update the scaling
		m_Scale += scaling;
	}

	//-------------------------------------------------------------------------------------

	void Transform::scale_by(float x_scaling, float y_scaling)
	{
		// Flag that the cached transform needs to be be updated
		m_CacheOutdated = true;

		// Update the scaling
		m_Scale.x += x_scaling;
		m_Scale.y += y_scaling;
	}

	//-------------------------------------------------------------------------------------

	void Transform::scale_to(float scale)
	{
		// Flag that the cached transform needs to be be updated
		m_CacheOutdated = true;

		// Update the scaling
		m_Scale.x = scale;
		m_Scale.y = scale;
	}

	//-------------------------------------------------------------------------------------

	void Transform::scale_to(const glm::vec2& scaling)
	{
		// Flag that the cached transform needs to be be updated
		m_CacheOutdated = true;

		// Update the scaling
		m_Scale = scaling;
	}

	//-------------------------------------------------------------------------------------

	void Transform::scale_to(float x_scaling, float y_scaling)
	{
		// Flag that the cached transform needs to be be updated
		m_CacheOutdated = true;

		// Update the scaling
		m_Scale.x = x_scaling;
		m_Scale.y = y_scaling;
	}

	//-------------------------------------------------------------------------------------

	void Transform::translate_towards(float x, float y, float amount)
	{
		// Project a vector from the current position to the given position
		// then scale it by the amount we want to move, and translate by it
		translate_by(glm::proj(m_Translation, glm::vec2(x, y)) * amount);
	}

	//-------------------------------------------------------------------------------------

	void Transform::translate_towards(float heading_degrees, float amount)
	{
		// Get the heading in radians, then use trigonometry to figure out 
		// how much we need to move in each direction
		const float heading_radians = to_radians(heading_degrees);
		translate_by(amount * sinf(heading_radians), amount * cosf(heading_radians));
	}

	//-------------------------------------------------------------------------------------

	void Transform::translate_towards(const glm::vec2& position, float amount)
	{
		// Project a vector from the current position to the given position
		// then scale it by the amount we want to move, and translate by it
		translate_by(glm::normalize(glm::proj(m_Translation, position)) * amount);
	}

	//-------------------------------------------------------------------------------------

	const glm::vec2& Transform::get_translation() const
	{
		return m_Translation;
	}

	//-------------------------------------------------------------------------------------

	float Transform::get_rotation_degrees() const
	{
		return to_degrees(m_Rotation);
	}

	//-------------------------------------------------------------------------------------

	float Transform::get_rotation_radians() const
	{
		return m_Rotation;
	}

	//-------------------------------------------------------------------------------------

	const glm::vec2& Transform::get_scale() const
	{
		return m_Scale;
	}

	//-------------------------------------------------------------------------------------

	glm::mat4 Transform::to_transform_matrix() const
	{
		// If the cache is outdated, then we need to update the matrix
		// otherwise just return the current matrix in the cache
		if(m_CacheOutdated)
		{
			// Update the cached transform matrix
			m_TransformCache = create_transform_2d(m_Translation, m_Scale, m_Rotation);

			// Clear the outdated flag on the cache
			m_CacheOutdated = false;
		}
		return m_TransformCache;
	}

	//-------------------------------------------------------------------------------------

}