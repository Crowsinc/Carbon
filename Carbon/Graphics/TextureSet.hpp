#pragma once

#include <variant>
#include <unordered_map>

#include "Resources/Texture.hpp"
#include "Resources/TextureAtlas.hpp"

namespace cbn
{
	
	class TextureSet
	{
	private:

		std::unordered_map<CKey<std::string>, std::variant<SRes<Texture>, SRes<TextureAtlas>>> m_Textures;
		std::unordered_map<CKey<std::string>, CKey<std::string>> m_SubTextures;
		std::unordered_map<CKey<std::string>, Texture::UVMap> m_UVMaps;
		std::map<TextureUnit, CKey<std::string>> m_Bindings;

	public:

		TextureSet() = default;

		void clear();

		void bind() const;

		bool is_bound() const;
	
		int used_unit_count() const;

		int total_unit_count() const;

		int unused_unit_count() const;

		void clear(const TextureUnit unit);

		bool is_set(const TextureUnit unit) const;

		bool contains(const CKey<std::string> key) const;

		TextureUnit get_unit(CKey<std::string> key) const;

		Texture::UVMap get_uv_map(const CKey<std::string> key) const;

		std::variant<SRes<Texture>, SRes<TextureAtlas>> get(const CKey<std::string> key) const;
		
		std::variant<SRes<Texture>, SRes<TextureAtlas>> get(const TextureUnit unit) const;
		
		void set(const TextureUnit unit, const CKey<std::string> key, const std::variant<SRes<Texture>, SRes<TextureAtlas>>& texture);

	};

}