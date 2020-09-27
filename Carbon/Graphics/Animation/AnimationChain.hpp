#pragma once

#include "AnimationNode.hpp"

namespace cbn
{

	class AnimationChain
	{
	private:

		AnimationNode m_StartingNode;
		AnimationNode* m_CurrentNode;

	public:

		AnimationChain(const AnimationNode&& start_node);

		AnimationChain(const AnimationNode& start_node);

		AnimationChain(const AnimationChain&& chain);
		
		AnimationChain(const AnimationChain& chain);

		void operator=(const AnimationChain& chain);

		void advance();

		void reset();

		bool can_advance() const;

		Name current_texture() const;

		AnimationNode current() const;

	};


}