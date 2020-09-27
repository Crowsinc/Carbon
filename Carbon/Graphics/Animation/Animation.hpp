#pragma once

#include <unordered_map>

#include "AnimationChain.hpp"

namespace cbn
{
	
	class Animation
	{
	private:

		std::unordered_map<Name, AnimationChain> m_AnimationChains;
		AnimationChain m_DefaultAnimationChain;
		AnimationChain* m_CurrentChain;

	public:

		Animation(const AnimationChain& default_chain);

		void remove(const Name& chain_name);

		void set_default(const AnimationChain& default_chain);
		
		void add(const Name& chain_name, const AnimationChain& animation_chain);

		bool contains(const Name& chain_name) const;
		
		AnimationChain get(const Name& chain_name) const;

		AnimationChain get_default() const;

		void reset_to_default();
		
		void reset();

		void advance();

		bool can_advance() const;

		void swap_to(const Name& chain_name);

		Name current_texture() const;

		AnimationNode current_node() const;

		AnimationChain current_chain() const;

	};

}