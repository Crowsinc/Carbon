#include "TextureSet.hpp"

namespace cbn
{
	
	//-------------------------------------------------------------------------------------

	void TextureSet::bind() const
	{
		for(const auto& [unit, key] : m_Bindings)
		{
			const auto& texture = m_Textures.at(key);

			if(std::holds_alternative<SRes<TextureAtlas>>(texture))
				std::get<SRes<TextureAtlas>>(texture)->bind(unit);
			else
				std::get<SRes<Texture>>(texture)->bind(unit);
		}
	}
	
	//-------------------------------------------------------------------------------------

	void TextureSet::clear()
	{
		m_SubTextures.clear();
		m_Textures.clear();
		m_Bindings.clear();
		m_UVMaps.clear();
	}
	
	//-------------------------------------------------------------------------------------

	bool TextureSet::is_bound() const
	{
		for(const auto& [unit, key] : m_Bindings)
		{
			const auto& texture = m_Textures.at(key);
			bool bound = false;

			if(std::holds_alternative<SRes<TextureAtlas>>(texture))
				bound = std::get<SRes<TextureAtlas>>(texture)->is_bound(unit);
			else
				bound = std::get<SRes<Texture>>(texture)->is_bound(unit);

			if(!bound)
				return false;
		}
		return true;
	}
	
	//-------------------------------------------------------------------------------------

	int TextureSet::used_unit_count() const
	{
		return m_Bindings.size();
	}
	
	//-------------------------------------------------------------------------------------

	int TextureSet::total_unit_count() const
	{
		return static_cast<int>(TextureUnit::UNIT_15) + 1;
	}
	
	//-------------------------------------------------------------------------------------

	int TextureSet::unused_unit_count() const
	{
		return total_unit_count() - used_unit_count();
	}
	
	//-------------------------------------------------------------------------------------

	void TextureSet::clear(const TextureUnit unit)
	{
		// Only clear if the unit is actually in use
		if(m_Bindings.count(unit) == 1)
		{
			const auto key = m_Bindings.at(unit);
			const auto texture = m_Textures.at(key);
			
			// If the texture is an atlas, we also need to remove
			// all its subtextures from the subtexture & UV maps. 
			if(std::holds_alternative<SRes<TextureAtlas>>(texture))
			{
				const auto atlas = std::get <SRes<TextureAtlas>>(texture);
				const auto subtexture_keys = atlas->get_sub_texture_keys();

				for(auto subtexture : subtexture_keys)
				{
					m_SubTextures.erase(subtexture);
					m_UVMaps.erase(subtexture);
				}
			}

			m_UVMaps.erase(key);
			m_Textures.erase(key);
			m_Bindings.erase(unit);
		}
	}
	
	//-------------------------------------------------------------------------------------

	bool TextureSet::is_set(const TextureUnit unit) const
	{
		return m_Bindings.count(unit) == 1;
	}
	
	//-------------------------------------------------------------------------------------

	bool TextureSet::contains(const CKey<std::string> key) const
	{
		return m_Textures.count(key) == 1 || m_SubTextures.count(key) == 1;
	}
	
	//-------------------------------------------------------------------------------------

	TextureUnit TextureSet::get_unit(CKey<std::string> key) const
	{
		CBN_Assert(contains(key), "Key is not associated with any textures in this set");

		// If the key is a subtexture, then we need to get its atlas key
		if(m_SubTextures.count(key) == 1)
		{
			key = m_SubTextures.at(key);
		}

		// Search through the bindings for the key
		for(const auto [unit, bound_key] : m_Bindings)
		{
			if(bound_key == key)
				return unit;
		}

		return {};
	}
	
	//-------------------------------------------------------------------------------------

	std::variant<SRes<Texture>, SRes<TextureAtlas>> TextureSet::get(const CKey<std::string> key) const
	{
		CBN_Assert(contains(key), "Key is not associated with any textures in this set");

		if(m_Textures.count(key) == 1)
			return m_Textures.at(key);
		else 
			return m_Textures.at(m_SubTextures.at(key));
	}
	
	//-------------------------------------------------------------------------------------

	std::variant<SRes<Texture>, SRes<TextureAtlas>> TextureSet::get(const TextureUnit unit) const
	{
		CBN_Assert(m_Bindings.count(unit), "Key is not associated with any textures in this set");
		
		return m_Textures.at(m_Bindings.at(unit));
	}

	//-------------------------------------------------------------------------------------

	Texture::UVMap TextureSet::get_uv_map(const CKey<std::string> key) const
	{
		CBN_Assert(contains(key), "Key is not associated with a texture within this set");

		return m_UVMaps.at(key);
	}
	
	//-------------------------------------------------------------------------------------

	void TextureSet::set(const TextureUnit unit, const CKey<std::string> key, const std::variant<SRes<Texture>, SRes<TextureAtlas>>& texture)
	{
		CBN_Assert(!contains(key), "Key is already associated with a texture within this set");

		// Clear the old unit which is being replaced
		clear(unit);

		// Add the texture to the binding, and the texture map
		m_Textures.insert({key, texture});
		m_Bindings.insert({unit, key});

		// We need to add the textures uvs to the UV map. Also if 
		// the texture is an atlas, we need to add its subtextures
		if(std::holds_alternative<SRes<TextureAtlas>>(texture))
		{
			const auto atlas = std::get <SRes<TextureAtlas>>(texture);
			const auto subtexture_keys = atlas->get_sub_texture_keys();

			for(auto subtexture : subtexture_keys)
			{
				const auto subtexture_data = atlas->get_sub_texture_data(subtexture);

				m_SubTextures.insert({subtexture, key});
				m_UVMaps.insert({subtexture, subtexture_data.uv_mapping});
			}
		}
		else
		{
			m_UVMaps.insert({key, std::get<SRes<Texture>>(texture)->uv_mapping()});
		}
	}
	
	//-------------------------------------------------------------------------------------

}