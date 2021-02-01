#pragma once

#include "Transforms/Translatable.hpp"
#include "Transforms/Rotatable.hpp"
#include "Transforms/Scalable.hpp"

namespace cbn
{
	
	//-------------------------------------------------------------------------------------

	class Transform : public Translatable2D, public Rotatable2D, public Scalable2D
	{
	public:

		Transform();
		
		Transform(const Transform& copy);

		Transform(const float x, const float y, const float rotation_degrees = 0, const float scale_x = 1, const float scale_y = 1);

		Transform(const glm::vec2& translation, const float rotation_degrees = 0, const glm::vec2& scale = {1, 1});

		void operator=(const Transform& copy);

		glm::vec2 apply(const glm::vec2& point) const;

		Transform apply(const Transform& other) const;

	};

	//-------------------------------------------------------------------------------------

}

