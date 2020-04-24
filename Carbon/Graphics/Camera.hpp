#pragma once

#include "../Maths/Transform.hpp"

namespace cbn
{
	
	class Camera 
	{
	private:

		constexpr static float s_MinimumZoomValue = 0.0001f;
		
		mutable bool m_ViewCacheOutdated, m_ProjectionCacheOutdated;
		mutable glm::mat4 m_ViewCache, m_ProjectionCache;
		
		glm::vec2 m_Resolution, m_CenterOffset;
		float m_Rotation, m_Zoom;
		glm::vec2 m_Translation;


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

		float get_zoom() const;

		float get_rotation_degrees() const;

		float get_rotation_radians() const;

		const glm::vec2& get_translation() const;

		const glm::vec2& get_resolution() const;

		glm::mat4 to_projection_matrix();

		glm::mat4 to_view_matrix();

	};


}