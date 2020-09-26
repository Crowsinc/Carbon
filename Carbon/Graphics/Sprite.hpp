#pragma once

#include <array>
#include <vector>
#include <optional>

#include "../Maths/Transform.hpp"
#include "../Utility/CachedKey.hpp"

namespace cbn
{

	class Sprite : public cbn::Transform
	{
	public:

		struct Textures
		{
			SKey main_texture = std::string{};
			std::optional<SKey> aux_texture_1;
			std::optional<SKey> aux_texture_2;
			std::optional<SKey> aux_texture_3;
		};

		struct AnimationNode
		{
			Textures textures;
			std::optional<AnimationNode> next;

			void operator=(const AnimationNode& other);
		};

		struct Tint
		{
			glm::u8vec4 ul_vertex;
			glm::u8vec4 ll_vertex;
			glm::u8vec4 lr_vertex;
			glm::u8vec4 ur_vertex;
		};

		struct Vertices
		{
			glm::vec2 ul_vertex;
			glm::vec2 ll_vertex;
			glm::vec2 lr_vertex;
			glm::vec2 ur_vertex;
		};

	private:

		glm::vec2 m_Size;
		Tint m_TintColour;
		Vertices m_LocalVertices;
		AnimationNode m_StartAnimationNode;
		AnimationNode m_CurrAnimationNode;

		void set_local_vertices(const glm::vec2 size);

	public:

		Sprite(const glm::vec2 size, const glm::vec2 position);

		void set_size(const glm::vec2 size);

		void set_tint(const Tint colour);

		void set_tint(const glm::u8vec4 colour);

		void set_textures(const Textures& sprite_textures);

		void set_animation(const AnimationNode node);

		void reset_animation();

		void advance_animation();
		
		Tint get_tint() const;

		glm::vec2 get_size() const;

		Textures get_textures() const;

		Vertices get_vertices() const;

		Vertices get_local_vertices() const;
	};

}