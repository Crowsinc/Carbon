#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/projection.hpp>
#include <algorithm>

#include "../Diagnostics/Assert.hpp"
#include "../Maths/Maths.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	Camera::Camera(const float width, const float height, const float x, const float y, const float zoom, const float rotation_degrees)
		: m_ViewCacheOutdated(true),
		m_ProjectionCacheOutdated(true),
		m_ViewCache(1.0),
		m_ProjectionCache(1.0),
		m_Resolution(0,0),
		m_CenterOffset(0, 0),
		m_Zoom(std::max(zoom, s_MinimumZoomValue)),
		m_Rotation(to_radians(rotation_degrees)),
		m_Translation(x,y)
	{
		// Set the initial resolution through its method so that the correct center offset is also set
		set_resolution(width, height);
	}

	
	//-------------------------------------------------------------------------------------

	Camera::Camera(const glm::vec2 & resolution, const glm::vec2 & translation, const float zoom, const float rotation_degrees)
		: m_ViewCacheOutdated(true),
		m_ProjectionCacheOutdated(true),
		m_ViewCache(1.0),
		m_ProjectionCache(1.0),
		m_Resolution(0,0),
		m_CenterOffset(0,0),
		m_Zoom(std::max(zoom, s_MinimumZoomValue)),
		m_Rotation(to_radians(rotation_degrees)),
		m_Translation(translation)
	{
		// Set the initial resolution through its method so that the correct center offset is also set
		set_resolution(resolution);
	}

	//-------------------------------------------------------------------------------------

	void Camera::translate_by(float x, float y)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Update the translation
		m_Translation.x += x;
		m_Translation.y += y;
	}

	//-------------------------------------------------------------------------------------

	void Camera::translate_by(const glm::vec2 & translation)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Update the translation
		m_Translation += translation;
	}

	//-------------------------------------------------------------------------------------

	void Camera::translate_to(float x, float y)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Update the translation
		m_Translation.x = x;
		m_Translation.y = y;
	}

	//-------------------------------------------------------------------------------------

	void Camera::translate_to(const glm::vec2 & position)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Update the translation
		m_Translation = position;
	}


	//-------------------------------------------------------------------------------------
	void Camera::translate_towards(float x, float y, float amount)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Project a vector from the current position to the given position
		// then scale it by the amount we want to move, and translate by it
		translate_by(glm::proj(m_Translation, glm::vec2(x, y)) * amount);
	}

	//-------------------------------------------------------------------------------------

	void Camera::translate_towards(float heading_degrees, float amount)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Get the heading in radians, then use trigonometry to figure out 
		// how much we need to move in each direction
		const float heading_radians = to_radians(heading_degrees);
		translate_by(amount * sinf(heading_radians), amount * cosf(heading_radians));
	}

	//-------------------------------------------------------------------------------------

	void Camera::translate_towards(const glm::vec2& position, float amount)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Project a vector from the current position to the given position
	    // then scale it by the amount we want to move, and translate by it
		translate_by(glm::normalize(glm::proj(m_Translation, position)) * amount);
	}

	//-------------------------------------------------------------------------------------

	void Camera::rotate_by(float degrees)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Update the rotation, making sure to convert to radians
		m_Rotation += to_radians(degrees);
	}

	//-------------------------------------------------------------------------------------

	void Camera::rotate_to(float degrees)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Update the rotation, making sure to convert to radians
		m_Rotation = to_radians(degrees);
	}

	//-------------------------------------------------------------------------------------

	void Camera::zoom_in_by(float zoom)
	{
		// Flag that the cached view matrices need to be be updated 
		m_ProjectionCacheOutdated = true;
		m_ViewCacheOutdated = true;

		// Update the zoom, noting that the sign of the zoom amount is switched so that a positive zoom value
		// will cause the camera to zoom in while a negative zoom amount will cause the camera to zoom out. 
		// This is explained further in the get_projection_matrix method.
		m_Zoom = std::max(s_MinimumZoomValue, m_Zoom - zoom);
	}

	//-------------------------------------------------------------------------------------

	void Camera::zoom_out_by(float zoom)
	{
		// Flag that the cached view matrices need to be be updated 
		m_ProjectionCacheOutdated = true;
		m_ViewCacheOutdated = true;

		// Update the zoom, keeping in mind that a bigger zoom value 
		// will result in a lower zoom level (zoom out).
		m_Zoom = std::max(s_MinimumZoomValue, m_Zoom + zoom);
	}

	//-------------------------------------------------------------------------------------

	void Camera::zoom_to(float zoom)
	{
		// Flag that the cached view matrices need to be be updated 
		m_ProjectionCacheOutdated = true;
		m_ViewCacheOutdated = true;

		// Update the zoom, keeping in mind that a bigger zoom value 
	    // will result in a lower zoom level (zoom out).
		m_Zoom = std::max(s_MinimumZoomValue, zoom);
	}

	//-------------------------------------------------------------------------------------

	void Camera::set_resolution(const float width, const float height)
	{
		set_resolution(glm::vec2(width, height));
	}

	//-------------------------------------------------------------------------------------

	void Camera::set_resolution(const glm::vec2 & resolution)
	{
		// Make sure the camera resolution is not negative and not 0
		CBN_Assert(resolution.x > 0 && resolution.y > 0, "Camera resolution must be greater than zero");

		// Update the camera resolution and calculate the center offset for this resolution. 
		// Where the center offset is the amount we have to offset the camera so that the translation,
		// and rotation of the camera relates to the middle of the camera view, while the coordinate
		// system still has (0,0) being in the bottom left of the camera when its translation is (0,0)
		m_CenterOffset.x = resolution.x / 2.0f;
		m_CenterOffset.y = -resolution.y / 2.0f;
		m_Resolution = resolution;

		// Since the view matrix will need to be updated, flag that the cache is out of date
		m_ViewCacheOutdated = true;
	}
	
	//-------------------------------------------------------------------------------------

	float Camera::get_zoom() const
	{
		return m_Zoom;
	}
	//-------------------------------------------------------------------------------------

	const glm::vec2& Camera::get_translation() const
	{
		return m_Translation;
	}
	//-------------------------------------------------------------------------------------

	float Camera::get_rotation_degrees() const
	{
		return to_degrees(m_Rotation);
	}
	//-------------------------------------------------------------------------------------

	float Camera::get_rotation_radians() const
	{
		return m_Rotation;
	}
	
	//-------------------------------------------------------------------------------------

	const glm::vec2& Camera::get_resolution() const
	{
		return m_Resolution;
	}
	
	//-------------------------------------------------------------------------------------

	glm::mat4 Camera::to_projection_matrix()
	{
		// If the cached projection matrix is outdated, then we need to update it
		// otherwise we can just return the current cached projection matrix
		if(m_ProjectionCacheOutdated)
		{
			// Simply create an orthographic matrix using glm, we do not
			// need to optimize anything for 2D here. This projection will 
			// make (0,0) in the bottom left, and (res_x, rex_y) in the top right
			// Where res_x and res_y are the camera resolution components multiplied
			// by the amount of zoom. A higher value of zoom will shrink the resolution
			// making the camera feel closer while a lower value of zoom will increase the
			// resolution making it feel further away. 
			m_ProjectionCache = build_orthographic_matrix(-m_CenterOffset.x * m_Zoom, m_CenterOffset.x * m_Zoom, m_CenterOffset.y * m_Zoom, -m_CenterOffset.y * m_Zoom);

			// Clear the outdated flag on the projection cache
			m_ProjectionCacheOutdated = false;
		}
		return m_ProjectionCache;
	}

	//-------------------------------------------------------------------------------------

	glm::mat4 Camera::to_view_matrix()
	{
		// If the cached view matrix is outdated, then we need to update it
		// otherwise we can just return the current cached view matrix
		if(m_ViewCacheOutdated)
		{
			// Update the view matrix, making sure to take into account the center offset
			m_ViewCache = build_view_matrix(m_Translation + m_CenterOffset, m_Rotation);
			
			// Clear the outdated flag on the view cache
			m_ViewCacheOutdated = false;
		}
		return m_ViewCache;
	}

	//-------------------------------------------------------------------------------------

}















