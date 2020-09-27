#pragma once

#include <optional>

#include "../../Utility/CachedKey.hpp"

namespace cbn
{

	struct AnimationNode
	{
		Name texture_name;
		std::optional<AnimationNode> next;

		AnimationNode(const AnimationNode& node);

		AnimationNode(const AnimationNode&& node);

		AnimationNode(const Name& texture_name, const std::optional<AnimationNode>& next = {});
		
		AnimationNode(const std::string& texture_name, const std::optional<AnimationNode>& next = {});

		AnimationNode(const char* texture_name, const std::optional<AnimationNode>& next = {});

		void operator=(const AnimationNode& other);

	};

}