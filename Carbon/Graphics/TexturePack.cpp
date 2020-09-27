#include "TexturePack.hpp"

#include "../Utility/VectorExtensions.hpp"

namespace cbn
{
  
    //-------------------------------------------------------------------------------------

    TexturePack::TexturePack(const std::unordered_map<Name, TextureEntry>& textures)
    {
        add(textures);
    }
    
    //-------------------------------------------------------------------------------------

    void TexturePack::add(const std::unordered_map<Name, TextureEntry>& textures)
    {
        // Simply defer to the individual add methods
        for(const auto& [texture_name, entry] : textures)
        {
            if(std::holds_alternative<SRes<TextureAtlas>>(entry))
                add(texture_name, std::get<SRes<TextureAtlas>>(entry));
            else
                add(texture_name, std::get<SRes<Texture>>(entry));
        }
    }
    
    //-------------------------------------------------------------------------------------

    void TexturePack::add(const Name& texture_name, const SRes<Texture>& texture)
    {
        CBN_Assert(!contains(texture_name), "Cannot add texture because the given name is already taken");
        CBN_Assert(texture != nullptr, "Texture is empty resource");

        const int texture_index = m_Textures.size();

        m_Textures.push_back(texture);
        m_TextureMap.insert(std::make_pair(texture_name, texture_index));
    }

    //-------------------------------------------------------------------------------------

    void TexturePack::add(const Name& atlas_name, const SRes<TextureAtlas>& atlas)
    {
        CBN_Assert(!contains(atlas_name), "Cannot add texture atlas because the given name is already taken");
        CBN_Assert(atlas != nullptr, "Texture atlas is empty resource");

        // Each atlas is backed by a normal texture. Unless we care about subtextures
        // we want to refer to each atlas as a texture so we will also add the backend 
        // texture individually to the texture pack
        add(atlas_name, atlas->as_texture());

        // Add atlas subtexture information
        const int atlas_index = m_TextureAtlases.size();
        m_TextureAtlases.push_back(atlas);
        m_TextureAtlasMap.insert(std::make_pair(atlas_name, atlas_index));

        for(const auto& subtexture : atlas->subtextures())
        {
            CBN_Assert(!contains(subtexture.texture_name), "Cannot add texture atlas because a subtexture name is already taken");
            
            m_SubTextureMap.insert(std::make_pair(subtexture.texture_name, atlas_name));
        }
    }
    
    //-------------------------------------------------------------------------------------

    void TexturePack::remove(const Name& texture_name)
    {
        CBN_Assert(contains(texture_name) && !is_subtexture(texture_name), "Given texture name is not associated with any textures in the pack");

        const int texture_index = m_TextureMap.at(texture_name);
        m_TextureMap.erase(texture_name);

        // We will remove the texture from the texture vector with a swap erase. This means that the texture
        // which was at the end of the vector will now be where the texture being removed was. Hence we need to
        // search for it in the texture map and update its index appropiately.

        swap_erase(m_Textures, texture_index);
        const int old_texture_index = m_Textures.size();
        const int new_texture_index = texture_index;

        for(const auto [name, index] : m_TextureMap)
        {
            if(index == old_texture_index)
            {
                m_TextureMap[name] = new_texture_index;
                break;
            }
        }

        // If the texture name refers to a texture atlas, we also need to remove the atlas components
        if(m_TextureAtlasMap.count(texture_name) == 1)
        {
            const int atlas_index = m_TextureAtlasMap.at(texture_name);
            const auto& atlas = m_TextureAtlases[atlas_index];
            const auto& subtextures = atlas->subtextures();

            // We are also removing the texture atlas with a swap erase, so the old index need to be updated too
            swap_erase(m_TextureAtlases, atlas_index);
            const int old_atlas_index = m_TextureAtlases.size();
            const int new_atlas_index = atlas_index;

            for(const auto [name, index] : m_TextureAtlasMap)
            {
                if(index == old_atlas_index)
                {
                    m_TextureAtlasMap[name] = new_atlas_index;
                    break;
                }
            }

            // Remove all map entries for the atlas and its subtextures
            m_TextureAtlasMap.erase(texture_name);
            for(const auto& subtexture : subtextures)
                m_SubTextureMap.erase(subtexture.texture_name);

        } 
    }
    
    //-------------------------------------------------------------------------------------

    TextureUVMap TexturePack::get_uvs(const Name & texture_name) const
    {
        CBN_Assert(contains(texture_name), "The given texture name is not associated with any textures in the pack");

        // If the texture is a subtexture, then we need to get the UV data from 
        // its parent texture atlas. Otherwise, we just treat it as a normal texture. 
        // Note that this works for texture names which reference an entire atlas because
        // we store the backend texture of each atlas as if it were a normal texture. 

        return is_subtexture(texture_name) ? get_subtexture(texture_name).uvs : get_texture(texture_name)->uvs();
    }
    
    //-------------------------------------------------------------------------------------

    SubTexture TexturePack::get_subtexture(const Name& texture_name) const
    {
        CBN_Assert(is_subtexture(texture_name), "The given texture name is not associated with any subtextures in the pack");

        return get_texture_atlas(resolve_texture_atlas(texture_name))->get_subtexture(texture_name);
    }
    
    //-------------------------------------------------------------------------------------

    SRes<Texture> TexturePack::get_texture(const Name& texture_name) const
    {
        CBN_Assert(contains(texture_name) && !is_subtexture(texture_name), "The given texture name is not associated with any textures in the pack");

        return m_Textures[m_TextureMap.at(texture_name)];
    }
    
    //-------------------------------------------------------------------------------------

    SRes<TextureAtlas> TexturePack::get_texture_atlas(const Name& texture_name) const
    {
        CBN_Assert(is_texture_atlas(texture_name), "The given texture name is not associated with any textures in the pack");

        return m_TextureAtlases[m_TextureAtlasMap.at(texture_name)];
    }
   
    //-------------------------------------------------------------------------------------

    Name TexturePack::resolve_texture_atlas(const Name& subtexture_name) const
    {
        CBN_Assert(is_subtexture(subtexture_name), "The given texture name is not associated with any subtextures in the pack");

        return m_SubTextureMap.at(subtexture_name);
    }
   
    //-------------------------------------------------------------------------------------

    bool TexturePack::contains(const Name& texture_name) const
    {
        // All atlases are also in the texture map with their backend 
        // textures so we dont need to check the texture atlas map
        return m_SubTextureMap.count(texture_name) == 1 || m_TextureMap.count(texture_name) == 1;
    }

    //-------------------------------------------------------------------------------------

    bool TexturePack::is_texture(const Name& texture_name) const
    {
        CBN_Assert(contains(texture_name), "Texture name is not associated with anything in the texture pack");

        return m_TextureMap.count(texture_name) == 1 && m_TextureAtlasMap.count(texture_name) == 0;
    }
    
    //-------------------------------------------------------------------------------------

    bool TexturePack::is_subtexture(const Name& texture_name) const
    {
        CBN_Assert(contains(texture_name), "Texture name is not associated with anything in the texture pack");

        return m_SubTextureMap.count(texture_name) == 1;
    }
    
    //-------------------------------------------------------------------------------------

    bool TexturePack::is_texture_atlas(const Name& texture_name) const
    {
        CBN_Assert(contains(texture_name), "Texture name is not associated with anything in the texture pack");

        return m_TextureAtlasMap.count(texture_name) == 1;
    }
    
    //-------------------------------------------------------------------------------------

    const std::vector<SRes<Texture>>& TexturePack::textures() const
    {
        return m_Textures;
    }
    
    //-------------------------------------------------------------------------------------

    const std::vector<SRes<TextureAtlas>>& TexturePack::texture_atlases() const
    {
        return m_TextureAtlases;
    }
    
    //-------------------------------------------------------------------------------------

    const std::vector<SubTexture>& TexturePack::subtextures(const Name& atlas_name) const
    {
        CBN_Assert(contains(atlas_name) && is_texture_atlas(atlas_name), "Given atlas name does not correspond to any texture atlases");
        
        return get_texture_atlas(atlas_name)->subtextures();
    }
    
    //-------------------------------------------------------------------------------------

    int TexturePack::subtexture_count() const
    {
        return m_SubTextureMap.size();
    }
    
    //-------------------------------------------------------------------------------------

    int TexturePack::texture_count() const
    {
        return m_Textures.size();
    }
    
    //-------------------------------------------------------------------------------------

}