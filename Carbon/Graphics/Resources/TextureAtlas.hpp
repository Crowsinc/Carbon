#pragma once

#include <unordered_map>
#include <filesystem>
#include <optional>
#include <vector>
#include <map>

#include "../../Data/Identity/Identifiable.hpp"
#include "../../Data/Identity/Identifier.hpp"
#include "../../Algorithms/BinPacking.hpp"
#include "../../Memory/Resource.hpp"
#include "Texture.hpp"
#include "Image.hpp"

namespace cbn
{

	/*
	TODO:
	struct SubImage
	{
		const bool rotated;
		const glm::vec2 position;
		const glm::vec2 resolution;
	}

	struct SubTexture : Identifiable
	{
		SubImage subimage;
		const TextureUVMap uvs;
	}

*/

	struct TexturePackingSettings
	{
		RectanglePackingHeuristic heuristic = RectanglePackingHeuristic::BLSF_BAF_SQR;
		bool allow_image_rotation = true;
		bool shrink_to_footprint = true;
	};

	struct SubTexture : Identifiable
	{
		const bool rotated;
		const TextureUVMap uvs;
		const glm::vec2 position;
		const glm::vec2 resolution;
		
		SubTexture(const Identifier& identity, const bool rotated, const TextureUVMap& uvs, const Rect<int>& rect);
	};

	class TextureAtlas
	{
	public:

		static SRes<TextureAtlas> Pack(const unsigned width, const unsigned height, const IdentityMap<SRes<Image>>& images, const TexturePackingSettings settings = {});

	private:

		std::unordered_map<Identifier, int> m_SubTextureMap;
		const std::vector<SubTexture> m_SubTextures;
		SRes<Texture> m_AtlasTexture;

		static TextureUVMap calculate_uvs(const Rect<int>& rect, const bool rotated, const glm::vec2& atlas_resolution);

		static glm::uvec2 determine_footprint(const std::vector<Rect<int>>& rectangles);

		static bool is_rotated(const Rect<int>& rect, const SRes<Image>& image); // This is kinda crap

		TextureAtlas(const SRes<Texture>& texture, const std::vector<SubTexture>& subtextures);
		
	public:

		void unbind() const;

		void bind(const TextureUnit texture_unit = TextureUnit::UNIT_0) const;

		bool is_bound(const TextureUnit texture_unit = TextureUnit::UNIT_0) const;

		TextureUVMap uvs() const;

		const unsigned width() const;

		const unsigned height() const;
		
		glm::uvec2 resolution() const;

		bool has_subtexture(const Identifier& subtexture) const;

		SubTexture get_subtexture(const Identifier& subtexture) const;

		const std::vector<SubTexture>& subtextures() const;

		void configure(const TextureSettings& settings);

		TextureSettings settings() const;

		SRes<Texture> as_texture() const;

	};

}