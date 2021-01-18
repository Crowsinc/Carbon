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

	Camera::Camera(const glm::vec2& resolution)
		: m_Resolution(resolution),
		m_BoundingBox(resolution),
		m_ViewMatrix(1.0f),
		m_ProjectionMatrix(1.0f),
		m_ViewProjectionMatrix(1.0f)
	{}
	
	//-------------------------------------------------------------------------------------

	Camera::Camera(const Transform& transform, const glm::vec2& resolution)
		: Transformable(transform),
		m_Resolution(resolution),
		m_BoundingBox(transform, resolution),
		m_ViewMatrix(1.0f),
		m_ProjectionMatrix(1.0f),
		m_ViewProjectionMatrix(1.0f) {}
	
	//-------------------------------------------------------------------------------------

	void Camera::resize(const glm::vec2& resolution)
	{
		//TODO: resize the bounding box or signal cache
		m_Resolution = resolution;
	}

	//-------------------------------------------------------------------------------------

	const glm::vec2& Camera::resolution() const
	{
		return m_Resolution;
	}
	
	//-------------------------------------------------------------------------------------

	const glm::mat4& Camera::view_matrix() const
	{
		//TODO: caching
		m_ViewMatrix = build_view_matrix(translation(), rotation_radians());

		return m_ViewMatrix;
	}
	
	//-------------------------------------------------------------------------------------

	const glm::mat4& Camera::projection_matrix() const
	{
		//TODO: caching
		const auto half_apparent_resolution = scale() * resolution() * 0.5f;
		m_ProjectionMatrix = build_orthographic_matrix(
			-half_apparent_resolution.x,
			 half_apparent_resolution.x,
			-half_apparent_resolution.y,
			 half_apparent_resolution.y
		);

		return m_ProjectionMatrix;
	}
	
	//-------------------------------------------------------------------------------------

	const glm::mat4& Camera::view_projection_matrix() const
	{
		//TODO: caching
		m_ViewProjectionMatrix = build_vp_matrix(view_matrix(), projection_matrix());

		return m_ViewProjectionMatrix;
	}
	
	//-------------------------------------------------------------------------------------

	const BoundingBox& Camera::bounding_box() const
	{
		//TODO: caching
		m_BoundingBox = BoundingBox{as_transform(), scale() * resolution()};

		return m_BoundingBox;
	}
	
	//-------------------------------------------------------------------------------------

}











