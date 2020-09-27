#pragma once

#include <vector>
#include <variant>
#include <unordered_map>

#include "Resources/TextureAtlas.hpp"
#include "Resources/Texture.hpp"

namespace cbn
{
	using TextureEntry = std::variant<Texture, TextureAtlas>;

	class TexturePack
	{
	private:

		std::vector<SRes<Texture>> m_Textures; // Contains textures and backend texture atlas textures
		std::vector<SRes<TextureAtlas>> m_TextureAtlases; // Contains texture atlases
		
		std::unordered_map<Name, int> m_TextureMap;
		std::unordered_map<Name, int> m_TextureAtlasMap;
		std::unordered_map<Name, Name> m_SubTextureMap;

	public:

		TexturePack() = default;

		TexturePack(const std::unordered_map<Name, TextureEntry>& textures);
		
		void add(const std::unordered_map<Name, TextureEntry>& textures);

		void add(const Name& texture_name, const SRes<Texture>& texture);

		void add(const Name& atlas_name, const SRes<TextureAtlas>& atlas);

		void remove(const Name& texture_name);

		TextureUVMap get_uvs(const Name& texture_name) const;

		SubTexture get_subtexture(const Name& texture_name) const;

		SRes<Texture> get_texture(const Name& texture_name) const;

		SRes<TextureAtlas> get_texture_atlas(const Name& texture_name) const;
		
		Name resolve_texture_atlas(const Name& subtexture_name) const;

		bool contains(const Name& texture_name) const;

		bool is_texture(const Name& texture_name) const;

		bool is_subtexture(const Name& texture_name) const;

		bool is_texture_atlas(const Name& texture_name) const;

		const std::vector<SRes<Texture>>& textures() const;
		
		const std::vector<SRes<TextureAtlas>>& texture_atlases() const;

		const std::vector<SubTexture>& subtextures(const Name& atlas_name) const;

		int subtexture_count() const;

		int texture_count() const;
		
	};


}