#include "Animator.hpp"

#include "../../Diagnostics/Assert.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	Animator::Animator(const Animation& default_animation)
		: m_DefaultAnimation(default_animation) {}

	//-------------------------------------------------------------------------------------

	void Animator::remove(const Name& animation_name)
	{
		CBN_Assert(contains(animation_name), "Animation does not contain a chain belonging to the given name");

		// If the chain being removed is the current chain, then we need to swap to the default chain
		if(m_CurrentAnimation == &m_Animations[animation_name])
		{
			reset_to_default();
		}
		m_Animations.erase(animation_name);
	}
	
	//-------------------------------------------------------------------------------------

	void Animator::set_default(const Animation& default_animation)
	{
		m_DefaultAnimation = default_animation;
	}
	
	//-------------------------------------------------------------------------------------

	void Animator::add(const Name& animation_name, const Animation& animation)
	{
		CBN_Assert(!contains(animation_name), "Animation already contains a chain belonging to the given name");

		m_Animations.insert(std::make_pair(animation_name, animation));
	}

	//-------------------------------------------------------------------------------------

	const Animation& Animator::get(const Name& animation_name) const
	{
		CBN_Assert(contains(animation_name), "Animation does not contain a chain belonging to the given name");
		
		return m_Animations.at(animation_name);
	}

	//-------------------------------------------------------------------------------------

	const Animation& Animator::get_default() const
	{
		return m_DefaultAnimation;
	}
	
	//-------------------------------------------------------------------------------------

	bool Animator::contains(const Name& animation_name) const
	{
		return m_Animations.count(animation_name) == 1;
	}

	//-------------------------------------------------------------------------------------

	void Animator::reset_to_default()
	{
		m_CurrentAnimation = &m_DefaultAnimation;
		reset();
	}

	//-------------------------------------------------------------------------------------

	void Animator::reset()
	{
		m_CurrentAnimation->reset();
	}

	//-------------------------------------------------------------------------------------

	void Animator::advance()
	{
		if(can_advance())
			m_CurrentAnimation->advance();
	}

	//-------------------------------------------------------------------------------------

	bool Animator::can_advance() const
	{
		return m_CurrentAnimation->can_advance();
	}

	//-------------------------------------------------------------------------------------

	void Animator::swap_to(const Name& animation_name)
	{
		CBN_Assert(contains(animation_name), "Animation does not contain a chain belonging to the given name");

		m_CurrentAnimation = &m_Animations[animation_name];
		reset();
	}

	//-------------------------------------------------------------------------------------

	const Name& Animator::current_texture() const
	{
		return m_CurrentAnimation->current_texture();
	}

	//-------------------------------------------------------------------------------------

	const AnimationNode& Animator::current_node() const
	{
		return m_CurrentAnimation->current();
	}
	
	//-------------------------------------------------------------------------------------

	const Animation& Animator::current_animation() const
	{
		return *m_CurrentAnimation;
	}

	//-------------------------------------------------------------------------------------

}