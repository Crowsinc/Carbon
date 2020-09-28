#pragma once

#include <unordered_map>

#include "Animation.hpp"

namespace cbn
{
	
	class Animator
	{
	private:

		std::unordered_map<Name, Animation> m_Animations;
		Animation* m_CurrentAnimation;
		Animation m_DefaultAnimation;

	public:

		Animator(const Animation& default_animation);

		void remove(const Name& animation_name);

		void set_default(const Animation& default_animation);
		
		void add(const Name& animation_name, const Animation& animation);

		bool contains(const Name& animation_name) const;
		
		const Animation& get(const Name& animation_name) const;

		const Animation& get_default() const;

		void reset_to_default();
		
		void reset();

		void advance();

		bool can_advance() const;

		void swap_to(const Name& animation_name);

		const Name& current_texture() const;

		const AnimationNode& current_node() const;

		const Animation& current_animation() const;

	};

}