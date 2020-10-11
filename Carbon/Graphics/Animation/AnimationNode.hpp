#pragma once

#include <functional>

#include "../../Utility/CachedKey.hpp"

namespace cbn
{
	// AnimationNode set up as a cons list
	struct AnimationNode
	{
		Name texture_name;
		std::function<AnimationNode()> next;

		AnimationNode(const AnimationNode& node);

		AnimationNode(const AnimationNode&& node);

		AnimationNode(const Name& texture_name);
		
		AnimationNode(const char* texture_name);

		AnimationNode(const std::string& texture_name);

		AnimationNode(const Name& texture_name, const AnimationNode& next);

		AnimationNode(const char* texture_name, const AnimationNode& next);

		AnimationNode(const std::string& texture_name, const AnimationNode& next);

		void operator=(const AnimationNode& other);

		bool has_next() const;

	};

}