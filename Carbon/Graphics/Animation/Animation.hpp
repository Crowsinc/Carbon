#pragma once

#include "AnimationNode.hpp"

namespace cbn
{

	class Animation
	{
	private:

		AnimationNode m_StartingNode;
		AnimationNode m_CurrentNode;

	public:

		Animation(const AnimationNode&& start_node);

		Animation(const AnimationNode& start_node);

		Animation(const Animation&& animation);
		
		Animation(const Animation& animation);

		void operator=(const Animation& animation);

		void advance();

		void reset();

		bool can_advance() const;

		const Name& current_texture() const;

		const AnimationNode& current() const;

	};


}