#pragma once

#include <unordered_map>
#include <filesystem>
#include <optional>
#include <vector>
#include <map>

#include "../../Algorithms/BinPacking.hpp"
#include "../OpenGL/TextureParameters.hpp"
#include "../../Utility/CachedKey.hpp"
#include "../../Memory/Resource.hpp"
#include "Texture.hpp"
#include "Image.hpp"

namespace cbn
{

	class TextureAtlas
	{
	public:

		struct PackingSettings
		{
			RectanglePackingHeuristic heuristic = RectanglePackingHeuristic::BLSF_BAF_SQR;
			bool allow_image_rotation = true;
			bool shrink_to_footprint = true;
		};

		static SRes<TextureAtlas> Pack(const unsigned width, const unsigned height, const PackingSettings settings, const std::map<CKey<std::string>,SRes<Image>> images);

		//TODO: static SRes<TextureAtlas> PackToFile(const unsigned width, const unsigned height, const PackingSettings settings, const std::map<CKey<std::string>, SRes<Image>> images);
		//TODO: static SRes<TextureAtlas> Open(const std::filesystem::path& path, Swizzle swizzle = Swizzle::RGBA);

	private:

		struct SubTextureData
		{
			bool rotated;
			Texture::UVMap uv_mapping;
			unsigned x, y, width, height;
		};
	
		std::unordered_map<CKey<std::string>, SubTextureData> m_SubTextureData;
		SRes<Texture> m_AtlasTexture;

		static Texture::UVMap calculate_uvs(const Rect<int>& rect, const bool rotated, const glm::uvec2& atlas_resolution);

		static bool is_rotated(const Rect<int>& rect, const SRes<Image>& image);

		static glm::uvec2 determine_footprint(const std::vector<Rect<int>>& rectangles);

		TextureAtlas(const SRes<Texture>& texture, const std::unordered_map<CKey<std::string>, SubTextureData>& sub_texture_data);
		
	public:


		void unbind() const;

		void bind(const Enum<TextureUnit> texture_unit = TextureUnit::UNIT_0) const;

		bool is_bound(const Enum<TextureUnit> texture_unit = TextureUnit::UNIT_0) const;

		Texture::UVMap uvs() const;

		const unsigned width() const;

		const unsigned height() const;
		
		glm::uvec2 resolution() const;

		bool has_sub_texture(const CKey<std::string>& key) const;

		std::vector<CKey<std::string>> get_sub_texture_keys() const;

		SubTextureData get_sub_texture_data(const CKey<std::string>& key) const;

		void configure(const Texture::Properties& properties);

		Texture::Properties get_properties() const;

		const SRes<Texture> as_texture() const;
		
	};

}