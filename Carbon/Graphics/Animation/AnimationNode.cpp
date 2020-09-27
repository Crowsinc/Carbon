#include "AnimationNode.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	AnimationNode::AnimationNode(const AnimationNode& node)
		: AnimationNode(node.texture_name, node.next) {}
	
	//-------------------------------------------------------------------------------------

	AnimationNode::AnimationNode(const AnimationNode&& node)
		: AnimationNode(node.texture_name, node.next) {}

	//-------------------------------------------------------------------------------------

	AnimationNode::AnimationNode(const Name& texture_name, const std::optional<AnimationNode>& next)
		: texture_name(texture_name), 
		next(next) {}

	//-------------------------------------------------------------------------------------

	AnimationNode::AnimationNode(const std::string& texture_name, const std::optional<AnimationNode>& next)
		: AnimationNode(Name{texture_name}, next) {}
	
	//-------------------------------------------------------------------------------------

	AnimationNode::AnimationNode(const char* texture_name, const std::optional<AnimationNode>& next)
		: AnimationNode(Name{texture_name}, next) {}
	
	//-------------------------------------------------------------------------------------

	void AnimationNode::operator=(const AnimationNode& other)
	{
		texture_name = other.texture_name;
		if(other.next.has_value())
			next = other.next.value();
		else 
			next.reset();
	}

	//-------------------------------------------------------------------------------------


}