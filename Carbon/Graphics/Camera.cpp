#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
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
		m_InterpolationComplete(true),
		m_MaximumZoom(0.0001f),
		m_LastZoom(std::max(zoom, m_MaximumZoom)),
		m_Zoom(std::max(zoom, m_MaximumZoom)),
		m_Transform(x,y,0,0,rotation_degrees)
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
		m_InterpolationComplete(true),
		m_MaximumZoom(0.0001f),
		m_LastZoom(std::max(zoom, m_MaximumZoom)),
		m_Zoom(std::max(zoom, m_MaximumZoom)),
		m_Transform(translation, {0,0}, rotation_degrees)
	{
		// Set the initial resolution through its method so that the correct center offset is also set
		set_resolution(resolution);
	}

	//-------------------------------------------------------------------------------------

	void Camera::translate_by(float x, float y)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Perform operation through the transform
		m_Transform.translate_by(x, y);
	}

	//-------------------------------------------------------------------------------------

	void Camera::translate_by(const glm::vec2 & translation)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Perform operation through the transform
		m_Transform.translate_by(translation);
	}

	//-------------------------------------------------------------------------------------

	void Camera::translate_to(float x, float y)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Perform operation through the transform
		m_Transform.translate_to(x,y);
	}

	//-------------------------------------------------------------------------------------

	void Camera::translate_to(const glm::vec2 & position)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Perform operation through the transform
		m_Transform.translate_to(position);
	}


	//-------------------------------------------------------------------------------------
	void Camera::translate_towards(float x, float y, float amount)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Perform operation through the transform
		m_Transform.translate_towards(x,y,amount);
	}

	//-------------------------------------------------------------------------------------

	void Camera::translate_towards(float heading_degrees, float amount)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Perform operation through the transform
		m_Transform.translate_towards(heading_degrees, amount);
	}

	//-------------------------------------------------------------------------------------

	void Camera::translate_towards(const glm::vec2& position, float amount)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Perform operation through the transform
		m_Transform.translate_towards(position, amount);
	}

	//-------------------------------------------------------------------------------------

	void Camera::rotate_by(float degrees)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Perform operation through the transform
		m_Transform.rotate_by(degrees);
	}

	//-------------------------------------------------------------------------------------

	void Camera::rotate_to(float degrees)
	{
		// Flag that the cached view matrix needs to be be updated
		m_ViewCacheOutdated = true;

		// Perform operation through the transform
		m_Transform.rotate_to(degrees);
	}

	//-------------------------------------------------------------------------------------

	void Camera::zoom_in_by(float zoom)
	{
		// Flag that the cached view matrix needs to be be updated and start interpolating
		m_ProjectionCacheOutdated = true;
		m_InterpolationComplete = false;
		m_ViewCacheOutdated = true;

		// Update the zoom, noting that the sign of the zoom amount is switched so that a positive zoom value
		// will cause the camera to zoom in while a negative zoom amount will cause the camera to zoom out. 
		// This is explained further in the get_projection_matrix method.
		m_Zoom -= zoom;

		// Clamp the zoom so that it doesn't go below the maximum zoom level
		m_Zoom = std::max(m_MaximumZoom, m_Zoom);
	}

	//-------------------------------------------------------------------------------------

	void Camera::zoom_out_by(float zoom)
	{
		// Flag that the cached view matrix needs to be be updated and start interpolating
		m_ProjectionCacheOutdated = true;
		m_InterpolationComplete = false;
		m_ViewCacheOutdated = true;

		// Update the zoom, keeping in mind that a bigger zoom value 
		// will result in a lower zoom level (zoom out).
		m_Zoom += zoom;

		// Clamp the zoom so that it doesn't go below the maximum zoom level
		m_Zoom = std::max(m_MaximumZoom, m_Zoom);
	}

	//-------------------------------------------------------------------------------------

	void Camera::zoom_to(float zoom)
	{
		// Make sure that the given zoom is greater than 0
		CBN_Assert(zoom >= m_MaximumZoom, "Zoom level must be greater than zero");

		// Flag that the cached projection matrix needs to be be updated
		m_ProjectionCacheOutdated = true;

		// Update the zoom value
		m_Zoom = zoom;

		// Clamp the zoom so that it doesn't go below the maximum zoom level
		m_Zoom = std::min(m_MaximumZoom, m_Zoom);

		// Set the last zoom to the new zoom, this is
		// because we don't want any interpolation on a jump
		m_LastZoom = m_Zoom;
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
		// system still has (0,0) being in the top left of the camera when its translation is (0,0)
		m_CenterOffset.x = resolution.x / 2.0f;
		m_CenterOffset.y = -resolution.y / 2.0f;
		m_Resolution = resolution;

		// Since the view matrix will need to be updated, flag that the cache is out of date
		m_ViewCacheOutdated = true;
	}
	
	//-------------------------------------------------------------------------------------

	void Camera::update_interpolation()
	{
		// Simply update last variables to the current ones
		m_Transform.update_interpolation();
		m_LastZoom = m_Zoom;

		// Update the interpolation flag
		m_InterpolationComplete = true;
	}

	//-------------------------------------------------------------------------------------

	float Camera::get_zoom() const
	{
		return m_Zoom;
	}
	//-------------------------------------------------------------------------------------

	const glm::vec2& Camera::get_translation() const
	{
		return m_Transform.get_translation();
	}
	//-------------------------------------------------------------------------------------

	float Camera::get_rotation_degrees() const
	{
		return m_Transform.get_rotation_degrees();
	}
	//-------------------------------------------------------------------------------------

	float Camera::get_rotation_radians() const
	{
		return m_Transform.get_rotation_radians();
	}
	//-------------------------------------------------------------------------------------

	float Camera::get_zoom(float interp_factor) const
	{
		// If the interpolation is complete, return the normal zoom
		if(is_interpolation_complete())
		{
			return get_zoom();
		}

		// Perform linear interpolation between the last and current zoom, then return the result
		return lerp(m_LastZoom, m_Zoom, interp_factor);
	}
	//-------------------------------------------------------------------------------------

	glm::vec2 Camera::get_translation(float interp_factor) const
	{
		// Note that this will automatically return the normal rotation if interpolation is complete
		return m_Transform.get_translation(interp_factor);
	}
	//-------------------------------------------------------------------------------------

	float Camera::get_rotation_degrees(float interp_factor) const
	{
		// Note that this will automatically return the normal rotation if interpolation is complete
		return m_Transform.get_rotation_degrees(interp_factor);
	}
	//-------------------------------------------------------------------------------------

	float Camera::get_rotation_radians(float interp_factor) const
	{
		// Note that this will automatically return the normal rotation if interpolation is complete
		return m_Transform.get_rotation_radians(interp_factor);
	}
	//-------------------------------------------------------------------------------------

	const glm::vec2& Camera::get_resolution() const
	{
		return m_Resolution;
	}
	
	//-------------------------------------------------------------------------------------

	bool Camera::is_interpolation_complete() const
	{
		return m_InterpolationComplete && m_Transform.is_interpolation_complete();
	}

	//-------------------------------------------------------------------------------------

	glm::mat3 Camera::to_projection_matrix()
	{
		// If the cached projection matrix is outdated, then we need to update it
		// otherwise we can just return the current cached projection matrix
		if(m_ProjectionCacheOutdated)
		{
			// Simply create an orthographic matrix using glm, we do not
			// need to optimize anything for 2D here. This projection will 
			// make (0,0) in the top left, and (res_x, rex_y) in the bottom right
			// Where res_x and res_y are the camera resolution components multiplied
			// by the amount of zoom. A higher value of zoom will shrink the resolution
			// making the camera feel closer while a lower value of zoom will increase the
			// resolution making it feel further away. 
			m_ProjectionCache = create_orthographic_2d(-m_CenterOffset.x * m_Zoom, m_CenterOffset.x * m_Zoom, -m_CenterOffset.y * m_Zoom, m_CenterOffset.y * m_Zoom);

			// Clear the outdated flag on the projection cache
			m_ProjectionCacheOutdated = false;
		}
		return m_ProjectionCache;
	}

	//-------------------------------------------------------------------------------------

	glm::mat3 Camera::to_projection_matrix(float interp_factor)
	{
		// If the interpolation has already been completed, simply use the normal projection matrix
		if(is_interpolation_complete())
		{
			return to_projection_matrix();
		}

		// Get the zoom interpolated zoom value
		const float zoom = get_zoom(interp_factor);

		// Simply create an orthographic matrix using glm, we do not
		// need to optimize anything for 2D here. This projection will 
		// make (0,0) in the top left, and (res_x, rex_y) in the bottom right
		// Where res_x and res_y are the camera resolution components multiplied
		// by the amount of zoom. A higher value of zoom will shrink the resolution
		// making the camera feel closer while a lower value of zoom will increase the
		// resolution making it feel further away. 
		return create_orthographic_2d(-m_CenterOffset.x * zoom, m_CenterOffset.x * zoom, -m_CenterOffset.y * zoom, m_CenterOffset.y * zoom);
	}
	
	//-------------------------------------------------------------------------------------

	glm::mat3 Camera::to_view_matrix()
	{
		// If the cached view matrix is outdated, then we need to update it
		// otherwise we can just return the current cached view matrix
		if(m_ViewCacheOutdated)
		{
			// Update the view matrix, making sure to take into account the center offset
			m_ViewCache = create_view_2d(m_Transform.get_translation() + m_CenterOffset, m_Transform.get_rotation_radians());
			
			// Clear the outdated flag on the view cache
			m_ViewCacheOutdated = false;
		}
		return m_ViewCache;
	}

	//-------------------------------------------------------------------------------------

	glm::mat4 Camera::to_view_matrix(float interp_factor)
	{
		// If the interpolation has already been completed, simply use the normal view matrix
		if(is_interpolation_complete())
		{
			return to_view_matrix();
		}

		// Retrieve the interpolated values from the transform, making sure to add
		// the center offset to the translation for the matrix calculations
		glm::vec2 translation = m_Transform.get_translation(interp_factor) + m_CenterOffset;
		const float rotation = m_Transform.get_rotation_radians(interp_factor);
		const float zoom = get_zoom(interp_factor);

		// Create a new view matrix with the interpolated values. 
		return create_view_2d(translation, rotation);
	}

	//-------------------------------------------------------------------------------------

}















