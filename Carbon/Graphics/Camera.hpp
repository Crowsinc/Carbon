#pragma once

#include "../Maths/Transform.hpp"

namespace cbn
{
	
	class Camera 
	{
	private:


		bool m_ViewCacheOutdated, m_ProjectionCacheOutdated;
		glm::mat3 m_ViewCache, m_ProjectionCache;
		glm::vec2 m_Resolution, m_CenterOffset;
		bool m_InterpolationComplete;
		const float m_MaximumZoom;
		float m_LastZoom, m_Zoom;
		Transform m_Transform;

	public:

		Camera(const float width, const float height, const float x = 0, const float y = 0, const float zoom = 1, const float rotation_degrees = 0);

		Camera(const glm::vec2& resolution, const glm::vec2& translation = {0,0}, const float zoom = 1, const float rotation_degrees = 0);

		void translate_by(float x, float y);

		void translate_by(const glm::vec2& translation);

		void translate_to(float x, float y);

		void translate_to(const glm::vec2& position);

		void translate_towards(float x, float y, float amount);

		void translate_towards(float heading_degrees, float amount);

		void translate_towards(const glm::vec2& position, float amount);

		void rotate_by(float degrees);

		void rotate_to(float degrees);

		void zoom_in_by(float zoom);
		
		void zoom_out_by(float zoom);

		void zoom_to(float zoom);

		void set_resolution(const float width, const float height);

		void set_resolution(const glm::vec2& resolution);

		void update_interpolation();

		float get_zoom() const;

		float get_rotation_degrees() const;

		float get_rotation_radians() const;

		const glm::vec2& get_translation() const;

		float get_zoom(float interp_factor) const;

		glm::vec2 get_translation(float interp_factor) const;

		float get_rotation_degrees(float interp_factor) const;

		float get_rotation_radians(float interp_factor) const;

		const glm::vec2& get_resolution() const;

		bool is_interpolation_complete() const;

		glm::mat3 to_projection_matrix(); // Note, not a real projection matrix

		glm::mat3 to_projection_matrix(float interp_factor); // Note, not a real projection matrix

		glm::mat3 to_view_matrix();

		glm::mat4 to_view_matrix(float interp_factor);

	};


}