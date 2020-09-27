#pragma once

#include <unordered_map>
#include <filesystem>
#include <optional>
#include <vector>
#include <map>

#include "../../Algorithms/BinPacking.hpp"
#include "../../Utility/CachedKey.hpp"
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

	struct SubTexture
	{
		SubImage subimage;
		const TextureUVMap uvs;
		const TextureName texture_name;
	}

*/

	struct TexturePackingSettings
	{
		RectanglePackingHeuristic heuristic = RectanglePackingHeuristic::BLSF_BAF_SQR;
		bool allow_image_rotation = true;
		bool shrink_to_footprint = true;
	};

	struct SubTexture
	{
		const bool rotated;
		const TextureUVMap uvs;
		const glm::vec2 position;
		const glm::vec2 resolution;
		const Name texture_name;
	};

	class TextureAtlas
	{
	public:

		static SRes<TextureAtlas> Pack(const unsigned width, const unsigned height, const std::unordered_map<Name, SRes<Image>>& images, const TexturePackingSettings settings = {});

	private:

		std::unordered_map<Name, int> m_SubTextureMap;
		const std::vector<SubTexture> m_SubTextures;
		SRes<Texture> m_AtlasTexture;

		static TextureUVMap calculate_uvs(const Rect<int>& rect, const bool rotated, const glm::vec2& atlas_resolution);

		static glm::uvec2 determine_footprint(const std::vector<Rect<int>>& rectangles);

		static bool is_rotated(const Rect<int>& rect, const SRes<Image>& image);

		TextureAtlas(const SRes<Texture>& texture, const std::vector<SubTexture>& subtextures);
		
	public:

		void unbind() const;

		void bind(const TextureUnit texture_unit = TextureUnit::UNIT_0) const;

		bool is_bound(const TextureUnit texture_unit = TextureUnit::UNIT_0) const;

		constexpr TextureUVMap uvs() const;

		const unsigned width() const;

		const unsigned height() const;
		
		glm::uvec2 resolution() const;

		bool has_subtexture(const Name& reference_name) const;

		SubTexture get_subtexture(const Name& reference_name) const;

		const std::vector<SubTexture>& subtextures() const;

		void set_properties(const TextureProperties& properties);

		TextureProperties get_properties() const;

		SRes<Texture> as_texture() const;

	};

}