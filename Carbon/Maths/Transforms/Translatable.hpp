#pragma once

#include <glm/glm.hpp>
#include <vector>


namespace cbn
{

	class Translatable2D 
	{
	private:

		glm::vec2 m_Translation;

	public:
		
		Translatable2D();

		Translatable2D(const glm::vec2& position);

		Translatable2D(const float x, const float y);

		void translate_by(const float x, const float y);
		
		void translate_by(const glm::vec2& translation);
		
		void translate_to(const float x, const float y);
		
		void translate_to(const glm::vec2& position);
		
		void translate_towards(const float x, const float y, const float amount);
		
		void translate_towards(const float heading_degrees, const float amount);
		
		void translate_towards(const glm::vec2& position, const float amount);

		const glm::vec2& translation() const;


	};

}