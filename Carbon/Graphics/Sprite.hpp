#pragma once

#include <array>
#include <vector>
#include <optional>

#include "../Maths/Transform.hpp"
#include "../Utility/CachedKey.hpp"

namespace cbn
{

	struct AnimationNode
	{
		SKey main_texture;
		std::optional<SKey> aux_texture_1;
		std::optional<SKey> aux_texture_2;
		std::optional<SKey> aux_texture_3;
		std::optional<AnimationNode> next;
	};


	class Sprite : public cbn::Transform
	{
	public:

		using Textures = std::tuple<SKey, std::optional<SKey>, std::optional<SKey>, std::optional<SKey>, std::optional<SKey>>;

	private:

		const glm::vec2 m_BaseSize;
		glm::u8vec4 m_TintColour;

		AnimationNode m_StartAnimationNode;
		AnimationNode m_CurrAnimationNode;
		unsigned m_CurrNodeFrames;

	public:

		Sprite(const glm::vec2 size);

		Textures get_textures() const;
		
		void set_tint(glm::u8vec4& colour);

		void set_texture(const SKey& main_texture);

		void set_texture(const SKey& main_texture, const SKey& aux_texture_1);
		
		void set_texture(const SKey& main_texture, const SKey& aux_texture_1, const SKey& aux_texture_2);
		
		void set_texture(const SKey& main_texture, const SKey& aux_texture_1, const SKey& aux_texture_2, const SKey& aux_texture_3);

		void set_animation(const AnimationNode& node);

		void advance_animation();

		void reset_animation();
		

	};

}