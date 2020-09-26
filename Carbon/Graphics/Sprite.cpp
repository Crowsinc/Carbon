#include "Sprite.hpp"

namespace cbn
{

	//-------------------------------------------------------------------------------------

	void Sprite::set_local_vertices(const glm::vec2 size)
	{
		const glm::vec2 half_size = size / 2.0f;
	
		m_LocalVertices.ul_vertex = {-half_size.x, half_size.y};
		m_LocalVertices.ll_vertex = -half_size;
		m_LocalVertices.lr_vertex = {half_size.x, -half_size.y};
		m_LocalVertices.ur_vertex = half_size;
	}

	//-------------------------------------------------------------------------------------

	Sprite::Sprite(const glm::vec2 size, const glm::vec2 position)
		: Transform(position),
		m_Size(size) 
	{
		set_local_vertices(size);
	}
	
	//-------------------------------------------------------------------------------------

	void Sprite::set_tint(const glm::u8vec4 colour)
	{
		m_TintColour.ul_vertex = colour;
		m_TintColour.ll_vertex = colour;
		m_TintColour.lr_vertex = colour;
		m_TintColour.ur_vertex = colour;
	}
	
	//-------------------------------------------------------------------------------------

	void Sprite::set_size(const glm::vec2 size)
	{
		m_Size = size;
		set_local_vertices(size);
	}
	
	//-------------------------------------------------------------------------------------

	void Sprite::set_tint(const Tint colour)
	{
		m_TintColour = colour;
	}
	
	//-------------------------------------------------------------------------------------

	void Sprite::set_textures(const Sprite::Textures& sprite_textures)
	{
		m_StartAnimationNode.textures = sprite_textures;
		m_StartAnimationNode.next.reset();

		m_CurrAnimationNode = m_StartAnimationNode;
	}

	//-------------------------------------------------------------------------------------
	
	void Sprite::set_animation(const AnimationNode node)
	{
		m_StartAnimationNode = node;
		m_CurrAnimationNode = node;
	}

	//-------------------------------------------------------------------------------------
	
	void Sprite::reset_animation()
	{
		m_CurrAnimationNode = m_StartAnimationNode;
	}

	//-------------------------------------------------------------------------------------
	
	void Sprite::advance_animation()
	{
		if(m_CurrAnimationNode.next.has_value())
			m_CurrAnimationNode = m_CurrAnimationNode.next.value();
	}

	//-------------------------------------------------------------------------------------
	
	glm::vec2 Sprite::get_size() const
	{
		m_Size;
	}

	//-------------------------------------------------------------------------------------

	Sprite::Tint Sprite::get_tint() const
	{
		return m_TintColour;
	}

	//-------------------------------------------------------------------------------------
	
	Sprite::Textures Sprite::get_textures() const
	{
		m_CurrAnimationNode.textures;
	}
	
	//-------------------------------------------------------------------------------------

	Sprite::Vertices Sprite::get_vertices() const
	{
		Vertices vertices = m_LocalVertices;
		vertices.ul_vertex += get_translation();
		vertices.ll_vertex += get_translation();
		vertices.lr_vertex += get_translation();
		vertices.ur_vertex += get_translation();
		return vertices;
	}
	
	//-------------------------------------------------------------------------------------

	Sprite::Vertices Sprite::get_local_vertices() const
	{
		return m_LocalVertices;
	}
	
	//-------------------------------------------------------------------------------------

	void Sprite::AnimationNode::operator=(const AnimationNode& other)
	{
		textures = other.textures;
		next = other.next.value();
	}

	//-------------------------------------------------------------------------------------

}