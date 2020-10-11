#include "Animation.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	Animation::Animation(const AnimationNode&& start_node)
		: m_StartingNode(start_node),
		m_CurrentNode(m_StartingNode) {}
	
	//-------------------------------------------------------------------------------------

	Animation::Animation(const AnimationNode& start_node)
		: m_StartingNode(start_node),
		m_CurrentNode(m_StartingNode) {}
	
	//-------------------------------------------------------------------------------------

	Animation::Animation(const Animation&& animation)
		: m_StartingNode(animation.m_StartingNode),
		m_CurrentNode(m_StartingNode) {}
	
	//-------------------------------------------------------------------------------------

	Animation::Animation(const Animation& animation)
		: m_StartingNode(animation.m_StartingNode),
		m_CurrentNode(m_StartingNode) {}
	
	//-------------------------------------------------------------------------------------

	void Animation::operator=(const Animation& animation)
	{
		m_StartingNode = animation.m_StartingNode;
		m_CurrentNode = m_StartingNode;
	}

	//-------------------------------------------------------------------------------------
	
	void Animation::advance()
	{
		if(can_advance())
			m_CurrentNode = m_CurrentNode.next();
	}

	//-------------------------------------------------------------------------------------

	void Animation::reset()
	{
		m_CurrentNode = m_StartingNode;
	}

	//-------------------------------------------------------------------------------------

	bool Animation::can_advance() const
	{
		return m_CurrentNode.has_next();
	}

	//-------------------------------------------------------------------------------------

	const Name& Animation::current_texture() const
	{
		return m_CurrentNode.texture_name;
	}
	
	//-------------------------------------------------------------------------------------

	const AnimationNode& Animation::current() const
	{
		return m_CurrentNode;
	}
	
	//-------------------------------------------------------------------------------------

}