#pragma once

#include "Maths.hpp"

namespace cbn
{

	class Transform
	{
	private:

		mutable glm::mat4 m_TransformCache;
		mutable bool m_CacheOutdated;
		
		glm::vec2 m_Translation, m_Scale;
		float m_Rotation;

	public:

		Transform(const float x = 0, const float y = 0, const float scale_x = 1, const float scale_y = 1, const float rotation_degrees = 0);

		Transform(const glm::vec2& translation = {0,0}, const glm::vec2& scale = {1,1}, const float rotation_degrees = 0);

		void translate_by(float x, float y);

		void translate_by(const glm::vec2& translation);

		void translate_to(float x, float y);

		void translate_to(const glm::vec2& position);

		void translate_towards(float x, float y, float amount);

		void translate_towards(float heading_degrees, float amount);

		void translate_towards(const glm::vec2& position, float amount);

		void rotate_by(float degrees);

		void rotate_to(float degrees);

		void scale_by(float scale);

		void scale_by(const glm::vec2& scaling);

		void scale_by(float x_scaling, float y_scaling);

		void scale_to(float scale);

		void scale_to(const glm::vec2& scaling);

		void scale_to(float x_scaling, float y_scaling);

		float get_rotation_degrees() const;

		float get_rotation_radians() const;

		const glm::vec2& get_scale() const;

		const glm::vec2& get_translation() const;

		glm::mat4 to_transform_matrix() const;
		
	};

}
