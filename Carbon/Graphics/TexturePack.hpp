#pragma once

#include <vector>
#include <variant>
#include <unordered_map>

#include "Resources/TextureAtlas.hpp"
#include "Resources/Texture.hpp"

namespace cbn
{

	using TextureEntry = std::variant<Texture, TextureAtlas>;
	using TextureName = CKey<std::string>;

	class TexturePack
	{
	private:

		std::vector<SRes<Texture>> m_Textures; // Contains textures and backend texture atlas textures
		std::vector<SRes<TextureAtlas>> m_TextureAtlases; // Contains texture atlases
		std::unordered_map<TextureName, int> m_TextureMap;
		std::unordered_map<TextureName, int> m_TextureAtlasMap;
		std::unordered_map<TextureName, TextureName> m_SubTextureMap;

	public:

		TexturePack() = default;

		TexturePack(const std::unordered_map<TextureName, TextureEntry>& textures);
		
		void add(const std::unordered_map<TextureName, TextureEntry>& textures);

		void add(const TextureName& texture_name, const SRes<Texture>& texture);

		void add(const TextureName& atlas_name, const SRes<TextureAtlas>& atlas);

		void remove(const TextureName& texture_name);

		TextureUVMap get_uvs(const TextureName& texture_name) const;

		SubTexture get_subtexture(const TextureName& texture_name) const;

		SRes<Texture> get_texture(const TextureName& texture_name) const;

		SRes<TextureAtlas> get_texture_atlas(const TextureName& texture_name) const;
		
		TextureName resolve_texture_atlas(const TextureName& subtexture_name) const;

		bool contains(const TextureName& texture_name) const;

		bool is_texture(const TextureName& texture_name) const;

		bool is_subtexture(const TextureName& texture_name) const;

		bool is_texture_atlas(const TextureName& texture_name) const;

		const std::vector<SRes<Texture>>& textures() const;
		
		const std::vector<SRes<TextureAtlas>>& texture_atlases() const;

		const std::vector<SubTexture>& subtextures(const TextureName& atlas_name) const;

		int subtexture_count() const;

		int texture_count() const;
		
	};


}