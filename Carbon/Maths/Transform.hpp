#pragma once

#include "Transforms/Translatable.hpp"
#include "Transforms/Rotatable.hpp"
#include "Transforms/Scalable.hpp"

namespace cbn
{
	
	//TODO: what is this class for? depicting transforms? if so do we need to be able to change the transform?
	// The transform is a way to depict a series of transformation. Perhaps we should have static constructors
	// for doing special transforms like look_at() etc.

	//-------------------------------------------------------------------------------------

	// Cannot use transformable as the transformable uses this and we would have an infinite loop
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

