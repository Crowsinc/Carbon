#include "Animation.hpp"

#include "../../Diagnostics/Assert.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	Animation::Animation(const AnimationChain& default_chain)
		: m_DefaultAnimationChain(default_chain) {}

	//-------------------------------------------------------------------------------------

	void Animation::remove(const Name& chain_name)
	{
		CBN_Assert(contains(chain_name), "Animation does not contain a chain belonging to the given name");

		// If the chain being removed is the current chain, then we need to swap to the default chain
		if(m_CurrentChain == &m_AnimationChains[chain_name])
		{
			reset_to_default();
		}
		m_AnimationChains.erase(chain_name);
	}
	
	//-------------------------------------------------------------------------------------

	void Animation::set_default(const AnimationChain& default_chain)
	{
		m_DefaultAnimationChain = default_chain;
	}
	
	//-------------------------------------------------------------------------------------

	void Animation::add(const Name& chain_name, const AnimationChain& animation_chain)
	{
		CBN_Assert(!contains(chain_name), "Animation already contains a chain belonging to the given name");

		m_AnimationChains.insert(std::make_pair(chain_name, animation_chain));
	}

	//-------------------------------------------------------------------------------------

	AnimationChain Animation::get(const Name& chain_name) const
	{
		CBN_Assert(contains(chain_name), "Animation does not contain a chain belonging to the given name");
		
		return m_AnimationChains.at(chain_name);
	}

	//-------------------------------------------------------------------------------------

	AnimationChain Animation::get_default() const
	{
		return m_DefaultAnimationChain;
	}
	
	//-------------------------------------------------------------------------------------

	bool Animation::contains(const Name& chain_name) const
	{
		return m_AnimationChains.count(chain_name) == 1;
	}

	//-------------------------------------------------------------------------------------

	void Animation::reset_to_default()
	{
		m_CurrentChain = &m_DefaultAnimationChain;
		reset();
	}

	//-------------------------------------------------------------------------------------

	void Animation::reset()
	{
		m_CurrentChain->reset();
	}

	//-------------------------------------------------------------------------------------

	void Animation::advance()
	{
		if(can_advance())
			m_CurrentChain->advance();
	}

	//-------------------------------------------------------------------------------------

	bool Animation::can_advance() const
	{
		return m_CurrentChain->can_advance();
	}

	//-------------------------------------------------------------------------------------

	void Animation::swap_to(const Name& chain_name)
	{
		CBN_Assert(contains(chain_name), "Animation does not contain a chain belonging to the given name");

		m_CurrentChain = &m_AnimationChains[chain_name];
		reset();
	}

	//-------------------------------------------------------------------------------------

	Name Animation::current_texture() const
	{
		return m_CurrentChain->current_texture();
	}

	//-------------------------------------------------------------------------------------

	AnimationNode Animation::current_node() const
	{
		return m_CurrentChain->current();
	}
	
	//-------------------------------------------------------------------------------------

	AnimationChain Animation::current_chain() const
	{
		return *m_CurrentChain;
	}

	//-------------------------------------------------------------------------------------

}