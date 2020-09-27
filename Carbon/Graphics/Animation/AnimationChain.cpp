#include "AnimationChain.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	AnimationChain::AnimationChain(const AnimationNode&& start_node)
		: m_StartingNode(start_node),
		m_CurrentNode(&m_StartingNode) {}
	
	//-------------------------------------------------------------------------------------

	AnimationChain::AnimationChain(const AnimationNode& start_node)
		: m_StartingNode(start_node),
		m_CurrentNode(&m_StartingNode) {}
	
	//-------------------------------------------------------------------------------------

	AnimationChain::AnimationChain(const AnimationChain&& chain)
		: m_StartingNode(chain.m_StartingNode),
		m_CurrentNode(&m_StartingNode) {}
	
	//-------------------------------------------------------------------------------------

	AnimationChain::AnimationChain(const AnimationChain& chain)
		: m_StartingNode(chain.m_StartingNode),
		m_CurrentNode(&m_StartingNode) {}
	
	//-------------------------------------------------------------------------------------

	void AnimationChain::operator=(const AnimationChain& chain)
	{
		m_StartingNode = chain.m_StartingNode;
		m_CurrentNode = &m_StartingNode;
	}

	//-------------------------------------------------------------------------------------
	
	void AnimationChain::advance()
	{
		if(can_advance())
			m_CurrentNode = &m_CurrentNode->next.value();
	}

	//-------------------------------------------------------------------------------------

	void AnimationChain::reset()
	{
		m_CurrentNode = &m_StartingNode;
	}

	//-------------------------------------------------------------------------------------

	bool AnimationChain::can_advance() const
	{
		return m_CurrentNode->next.has_value();
	}

	//-------------------------------------------------------------------------------------

	Name AnimationChain::current_texture() const
	{
		return m_CurrentNode->texture_name;
	}
	
	//-------------------------------------------------------------------------------------

	AnimationNode AnimationChain::current() const
	{
		return *m_CurrentNode;
	}
	
	//-------------------------------------------------------------------------------------

}