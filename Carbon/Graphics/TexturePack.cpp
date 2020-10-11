#include "TexturePack.hpp"

namespace cbn
{

    /*
        NOTES
            Buffer textures are only guaranteed to be able to store a maximum of 65536 texels minimum. This means that
            technically texture pack creation can fail, if we have more than 16384 textures/subtextures and are on old
            hardware. However its incredibly unlikely that a game would even have that many textures, and that the limit
            would even be 65536 anyway. My gpu currently supports 134217728 texels. 

            Possible Optimisations to Space (and Cache?):
                - Smaller data types. This is possible, since we do not actually need a full 32bit float for the
                data. Specially since the texture index only goes up to 15 currently or 31 in the future. The only 
                downside is maybe we have to divide in the shader, or have to deal with half floats but its
                not really an issue tbh. 
    
    */

    //-------------------------------------------------------------------------------------

    TexturePack::DataLayout TexturePack::pack_data(const TextureUVMap& uvs, const uint64_t texture_index)
    {
        const glm::vec3 texture_data_1 = {
            uvs.uv_1.x,
            uvs.uv_1.y,
            texture_index
        };

        const glm::vec3 texture_data_2 = {
            uvs.uv_2.x,
            uvs.uv_2.y,
            texture_index
        };

        const glm::vec3 texture_data_3 = {
            uvs.uv_3.x,
            uvs.uv_3.y,
            texture_index
        };

        const glm::vec3 texture_data_4 = {
            uvs.uv_4.x,
            uvs.uv_4.y,
            texture_index
        };

        return {texture_data_1,texture_data_2,texture_data_3,texture_data_4};
    }
    
    //-------------------------------------------------------------------------------------

    void TexturePack::initialize(const std::array<TexturePackEntry, SupportedTextureCount>& textures)
    {
        std::vector<DataLayout> buffer_texture_data;

        // Record texture data and set up the data for the buffer texture
        for(const auto [name, entry] : textures)
        {
            // If the entry is empty, skip this iteration
            if(std::holds_alternative<std::monostate>(entry))
                continue;

            SRes<Texture> texture;
            auto uv_index = m_TextureUVs.size();
            auto data_index = buffer_texture_data.size();
            const auto texture_index = m_Textures.size();

            if(std::holds_alternative<SRes<TextureAtlas>>(entry))
            {
                const auto& atlas = std::get<SRes<TextureAtlas>>(entry);

                // If we have a texture atlas, we need to individually add each subtexture's uvs.
                // We also want to texture equal to the backing texture of the atlas so that
                // it is added as if it were a normal texture. 

                const auto& subtextures = atlas->subtextures();
                for(const auto& subtexture : subtextures)
                {
                    CBN_Assert(m_TextureLookupMap.count(subtexture.texture_name) == 0, "Duplicate texture name found");

                    // Add subtexture name and uv data
                    m_TextureUVs.push_back(subtexture.uvs);
                    m_UVLookupMap[subtexture.texture_name] = uv_index++;
                    m_TextureLookupMap[subtexture.texture_name] = texture_index;

                    // Add subtexture data to the buffer texture data
                    buffer_texture_data.push_back(pack_data(subtexture.uvs, texture_index));
                    m_DataIndexLookupMap[subtexture.texture_name] = 4 * data_index;
                    data_index++;

                    // Set the backing texture
                    texture = atlas->as_texture();
                }
            }
            else texture = std::get<SRes<Texture>>(entry);

            CBN_Assert(m_TextureLookupMap.count(name) == 0, "Duplicate texture name found");

            // Add the normal texture name and uv data
            const auto texture_uvs = texture->uvs();
            m_TextureUVs.push_back(texture_uvs);
            m_UVLookupMap[name] = uv_index;

            m_Textures.push_back(texture);
            m_TextureLookupMap[name] = texture_index;

            // Add texture data to the buffer texture data
            buffer_texture_data.push_back(pack_data(texture_uvs, texture_index));
            m_DataIndexLookupMap[name] = 4 * data_index;
            data_index++;
        }

        // If no textures were actually supplied, then the texture pack is empty
        m_Empty = buffer_texture_data.empty();

        // Upload data to the buffer texture, unless its empty
        if(!m_Empty)
            m_BufferTexture = BufferTexture::Allocate(reinterpret_cast<uint8_t*>(buffer_texture_data.data()), buffer_texture_data.size() * sizeof(DataLayout), BufferTextureDataFormat::VEC3_F, m_OpenGLVersion);
    }
    
    //-------------------------------------------------------------------------------------

    TexturePack::TexturePack(const Version& opengl_version)
        : m_OpenGLVersion(opengl_version),
        m_Empty(true) {}
    
    //-------------------------------------------------------------------------------------

    TexturePack::TexturePack(const std::array<TexturePackEntry, SupportedTextureCount>& textures, const Version& opengl_version)
        : m_OpenGLVersion(opengl_version)
    {
        initialize(textures);
    }

    //-------------------------------------------------------------------------------------

    TexturePack::TexturePack(const TexturePack& other)
        : m_OpenGLVersion(other.m_OpenGLVersion),
        m_BufferTexture(other.m_BufferTexture),
        m_TextureNames(other.m_TextureNames),
        m_Textures(other.m_Textures),
        m_Empty(other.m_Empty),
        m_TextureUVs(other.m_TextureUVs),
        m_UVLookupMap(other.m_UVLookupMap),
        m_TextureLookupMap(other.m_TextureLookupMap),
        m_DataIndexLookupMap(other.m_DataIndexLookupMap)
    {}

    //-------------------------------------------------------------------------------------

    void TexturePack::operator=(const TexturePack& other)
    {
        m_DataIndexLookupMap = other.m_DataIndexLookupMap;
        m_TextureLookupMap = other.m_TextureLookupMap;
        m_OpenGLVersion = other.m_OpenGLVersion;
        m_BufferTexture = other.m_BufferTexture;
        m_TextureNames = other.m_TextureNames;
        m_UVLookupMap = other.m_UVLookupMap;
        m_TextureUVs = other.m_TextureUVs;
        m_Textures = other.m_Textures;
        m_Empty = other.m_Empty;
    }
    
    //-------------------------------------------------------------------------------------

    void TexturePack::operator=(const std::array<TexturePackEntry, SupportedTextureCount>& textures)
    {
        // Clear all current texture data 
        m_Textures.clear();
        m_TextureUVs.clear();
        m_TextureNames.clear();

        m_UVLookupMap.clear();
        m_TextureLookupMap.clear();
        m_DataIndexLookupMap.clear();

        // re-initialize the texture pack
        initialize(textures);
    }

    //-------------------------------------------------------------------------------------

    const SRes<Texture> TexturePack::texture_of(const Name& texture_name) const
    {
        CBN_Assert(contains(texture_name), "No texture with the given name exists");

        return m_Textures[m_TextureLookupMap.at(texture_name)];
    }
    
    //-------------------------------------------------------------------------------------

    const TextureUVMap TexturePack::uvs_of(const Name& texture_name) const
    {
        CBN_Assert(contains(texture_name), "No texture with the given name exists");
        
        return m_TextureUVs[m_TextureLookupMap.at(texture_name)];
    }
    
    //-------------------------------------------------------------------------------------

    uint32_t TexturePack::position_of(const Name& texture_name) const
    {
        CBN_Assert(contains(texture_name), "No texture with the given name exists");

        return m_DataIndexLookupMap.at(texture_name);
    }
    
    //-------------------------------------------------------------------------------------

    const std::vector<SRes<Texture>> TexturePack::textures() const
    {
        return m_Textures;
    }

    //-------------------------------------------------------------------------------------

    const std::vector<Name> TexturePack::texture_names() const
    {
        return m_TextureNames;
    }

    //-------------------------------------------------------------------------------------

    const std::vector<TextureUVMap>& TexturePack::uvs() const
    {
        return m_TextureUVs;
    }

    //-------------------------------------------------------------------------------------

    bool TexturePack::contains(const Name& texture_name) const
    {
        return m_TextureLookupMap.count(texture_name);
    }

    //-------------------------------------------------------------------------------------

    int TexturePack::texture_resource_count() const
    {
        return m_Textures.size();
    }

    //-------------------------------------------------------------------------------------

    int TexturePack::texture_count() const
    {
        return m_TextureLookupMap.size();
    }
    
    //-------------------------------------------------------------------------------------

    bool TexturePack::is_empty() const
    {
        return m_Empty;
    }

    //-------------------------------------------------------------------------------------

    bool TexturePack::is_bound() const
    {
        // The buffer texture is always bound to texture unit 0, then the rest of the textures are bound in the 
        // remaining units. In the order that they are stored in the m_Textures vector. They must be bound in 
        // this exact order, otherwise the data in the buffer texture will not match up in the shader. 

        GLint texture_unit_offset = 1;
        return !m_Empty && m_BufferTexture->is_bound(TextureUnit::UNIT_0) && std::all_of(m_Textures.begin(), m_Textures.end(), [&](const auto& texture)
        {
            return texture->is_bound(static_cast<TextureUnit>(static_cast<int>(TextureUnit::UNIT_0) + texture_unit_offset++));
        });
    }

    //-------------------------------------------------------------------------------------

    void TexturePack::unbind() const
    {
        CBN_Assert(!is_empty(), "Cannot unbind an empty texture pack");

        m_BufferTexture->unbind();
        for(const auto& texture : m_Textures)
            texture->unbind();
    }

    //-------------------------------------------------------------------------------------

    void TexturePack::bind() const
    {
        CBN_Assert(!is_empty(), "Cannot bind an empty texture pack");

        // The buffer texture is always bound to texture unit 0, then the rest of the textures are bound in the 
        // remaining units. In the order that they are stored in the m_Textures vector. They must be bound in 
        // this exact order, otherwise the data in the buffer texture will not match up in the shader. 

        m_BufferTexture->bind(TextureUnit::UNIT_0);

        GLint texture_unit_offset = 1;
        for(const auto& texture : m_Textures)
            texture->bind(static_cast<TextureUnit>(static_cast<GLint>(TextureUnit::UNIT_0) + texture_unit_offset++));
    }

    //-------------------------------------------------------------------------------------

}