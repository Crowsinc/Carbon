#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/projection.hpp>
#include <algorithm>

#include "../Diagnostics/Assert.hpp"
#include "../Maths/Matrix.hpp"
#include "../Maths/Maths.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	void Camera::on_transform()
	{
		// With the transform of the camera changed, we need to flag all the caches as being outdated.
		m_ViewMatrixOutdated = true;
		m_BoundingBoxOutdated = true;
		m_ProjectionMatrixOutdated = true;
		m_ViewProjectionMatrixOutdated = true;
	}

	//-------------------------------------------------------------------------------------

	Camera::Camera(const glm::vec2& resolution)
		: m_Resolution(resolution),
		m_BoundingBox(resolution),
		m_ViewMatrix(1.0f),
		m_ProjectionMatrix(1.0f),
		m_ViewProjectionMatrix(1.0f),
		m_ViewMatrixOutdated(true),
		m_BoundingBoxOutdated(true),
		m_ProjectionMatrixOutdated(true),
		m_ViewProjectionMatrixOutdated(true)
	{}
	
	//-------------------------------------------------------------------------------------

	Camera::Camera(const Transform& transform, const glm::vec2& resolution)
		: Transformable(transform),
		m_Resolution(resolution),
		m_BoundingBox(transform, resolution),
		m_ViewMatrix(1.0f),
		m_ProjectionMatrix(1.0f),
		m_ViewProjectionMatrix(1.0f),
		m_ViewMatrixOutdated(true),
		m_BoundingBoxOutdated(true),
		m_ProjectionMatrixOutdated(true),
		m_ViewProjectionMatrixOutdated(true)
	{}
	
	//-------------------------------------------------------------------------------------

	void Camera::resize(const glm::vec2& resolution)
	{
		m_Resolution = resolution;

		// The bounding box will need to be resized from the resolution change
		m_BoundingBoxOutdated = true;
	}

	//-------------------------------------------------------------------------------------

	const glm::vec2& Camera::resolution() const
	{
		return m_Resolution;
	}
	
	//-------------------------------------------------------------------------------------

	const glm::mat4& Camera::view_matrix() const
	{
		if(m_ViewMatrixOutdated)
		{
			m_ViewMatrix = build_view_matrix(translation(), rotation_radians());

			m_ViewMatrixOutdated = false;
		}

		return m_ViewMatrix;
	}
	
	//-------------------------------------------------------------------------------------

	const glm::mat4& Camera::projection_matrix() const
	{
		if(m_ProjectionMatrixOutdated)
		{
			const auto half_apparent_resolution = scale() * resolution() * 0.5f;
			m_ProjectionMatrix = build_orthographic_matrix(
				-half_apparent_resolution.x,
				 half_apparent_resolution.x,
				-half_apparent_resolution.y,
				 half_apparent_resolution.y
			);

			m_ProjectionMatrixOutdated = false;
		}

		return m_ProjectionMatrix;
	}
	
	//-------------------------------------------------------------------------------------

	const glm::mat4& Camera::view_projection_matrix() const
	{
		if(m_ViewProjectionMatrixOutdated)
		{
			m_ViewProjectionMatrix = build_vp_matrix(view_matrix(), projection_matrix());

			m_ViewProjectionMatrixOutdated = false;
		}

		return m_ViewProjectionMatrix;
	}
	
	//-------------------------------------------------------------------------------------

	const BoundingBox& Camera::bounding_box() const
	{
		if(m_BoundingBoxOutdated)
		{
			m_BoundingBox.transform_to(as_transform());
		    m_BoundingBox.resize(scale() * resolution());

			m_BoundingBoxOutdated = true;
		}

		return m_BoundingBox;
	}
	
	//-------------------------------------------------------------------------------------

}











