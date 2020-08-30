#include "TextureAtlas.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>

#include "../../Maths/Maths.hpp"
#include "Texture.hpp"

namespace cbn
{
    
    //-------------------------------------------------------------------------------------

    Texture::UVMap TextureAtlas::calculate_uvs(const Rect<int>& rect, const bool rotated, const glm::uvec2& atlas_resolution)
    {
        Texture::UVMap uvs;
        uvs.ul = {rect.x, rect.y};
        uvs.ll = {rect.x, rect.y + rect.height};
        uvs.lr = {rect.x + rect.width, rect.y + rect.height};
        uvs.ur = {rect.x + rect.width,  rect.y};

        uvs.ul /= atlas_resolution;
        uvs.ll /= atlas_resolution;
        uvs.lr /= atlas_resolution;
        uvs.ur /= atlas_resolution;

        // If the texture is rotated clockwise by 90 degrees, we need to counter rotate the uvs
        if(rotated)
        {
            const glm::vec2 temp_ul = uvs.ul;
            uvs.ul = uvs.ll;
            uvs.ll = uvs.lr;
            uvs.lr = uvs.ur;
            uvs.ur = temp_ul;
        }

        return uvs;
    }
    
    //-------------------------------------------------------------------------------------

    bool TextureAtlas::is_rotated(const Rect<int>& rect, const SRes<Image>& image)
    {
        return rect.width != rect.height && rect.width == image->height() && rect.height == image->width();
    }
    
    //-------------------------------------------------------------------------------------

    glm::uvec2 TextureAtlas::determine_footprint(const std::vector<Rect<int>>& rectangles)
    {
        const auto max_horz_rect = std::max_element(rectangles.begin(), rectangles.end(), [](const Rect<int> left, const Rect<int> right)
        {
            return left.x + left.width < right.x + right.width;
        });

        const auto max_vert_rect = std::max_element(rectangles.begin(), rectangles.end(), [](const Rect<int> left, const Rect<int> right)
        {
            return left.y + left.height < right.y + right.height;
        });

        return {max_horz_rect->x + max_horz_rect->width, max_vert_rect->y + max_vert_rect->height};
    }

    //-------------------------------------------------------------------------------------
    
    SRes<TextureAtlas> TextureAtlas::Pack(const unsigned width, const unsigned height, const PackingSettings settings, const std::map<CKey<std::string>, SRes<Image>> images)
    {
        // Create rectangles representing the images, these will be used 
        // in a rectangle packing algorithm to represent the textures. 
        // The rectangles will correspond with the texture of the same index. 
        // Note that out map is ordered, so we always iterate over it the same way.
        std::vector<Rect<int>> rectangles(images.size());
        std::transform(images.begin(), images.end(), rectangles.begin(), [&](const std::pair<CKey<std::string>, SRes<Image>>& pair)
        {
            return Rect<int>{0, 0, static_cast<int>(pair.second->width()), static_cast<int>(pair.second->height())};
        });

        // Use the MaxRects algorithm to pack the texture rectangles optimally
        // The algorithm will update the x and y of the rectangles inside the rectangles vector
        if(max_rects_optimal({width, height}, settings.allow_image_rotation, settings.heuristic, rectangles)) {
 
            // If we want to shrink to footprint then we need to find the extent of the rectangles,
            // and push the extent to the next power of 2, this will be the atas size we use.
            glm::uvec2 atlas_resolution = {width, height};
            if(settings.shrink_to_footprint)
            {
                const glm::uvec2 footprint = determine_footprint(rectangles);

                // The next power of 2 of the footprint will be used as the size
                atlas_resolution.x = std::min(width, next_power(footprint.x, 2u));
                atlas_resolution.y = std::min(height, next_power(footprint.y, 2u));
            }

            SRes<Image> atlas_image = Image::Create(atlas_resolution.x, atlas_resolution.y);
            if(!atlas_image) return nullptr;

            // Insert images and generate references. The references 
            // and rectangles are in the order of the images map
            int i = 0;
            std::unordered_map<CKey<std::string>,SubTextureData> sub_texture_data;
            for(auto const& [key, image] : images)
            {
                SubTextureData sub_texture;
                const auto& rect = rectangles[i];
                i++;
                
                sub_texture.x = rect.x;
                sub_texture.y = rect.y;
                sub_texture.width = rect.width;
                sub_texture.height = rect.height;
                sub_texture.rotated = is_rotated(rect, image);
                sub_texture.uv_mapping = calculate_uvs(rect, is_rotated(rect, image), atlas_image->resolution());
                sub_texture_data[key] = sub_texture;
            
                atlas_image->insert(sub_texture.x, sub_texture.y, image, sub_texture.rotated);
            }

            // Create the texture
            Texture::Properties properties;
            properties.minifying_filter = TextureFilter::NEAREST;
            properties.magnifying_filter = TextureFilter::NEAREST;
            properties.vertical_wrapping = TextureWrapping::CLAMP_TO_EDGE;
            properties.horizontal_wrapping = TextureWrapping::CLAMP_TO_EDGE;
            SRes<Texture> atlas_texture = Texture::Create(atlas_image, properties);
            
            if(!atlas_texture)
                return nullptr;

            return Resource::WrapShared<TextureAtlas>(new TextureAtlas(atlas_texture, sub_texture_data));
        }
        else return nullptr;
    }
    //-------------------------------------------------------------------------------------
   
    TextureAtlas::TextureAtlas(const SRes<Texture>& texture, const std::unordered_map<CKey<std::string>, SubTextureData>& sub_texture_data)
        : m_AtlasTexture(texture),
        m_SubTextureData(sub_texture_data) {}
    
    //-------------------------------------------------------------------------------------

    void TextureAtlas::bind(const TextureUnit texture_unit) const
    {
        return m_AtlasTexture->bind(texture_unit);
    }
    
    //-------------------------------------------------------------------------------------

    void TextureAtlas::unbind() const
    {
        return m_AtlasTexture->unbind();
    }

    //-------------------------------------------------------------------------------------

    bool TextureAtlas::is_bound(const TextureUnit texture_unit) const
    {
        return m_AtlasTexture->is_bound(texture_unit);
    }
    
    //-------------------------------------------------------------------------------------

    Texture::UVMap TextureAtlas::uvs() const
    {
        return m_AtlasTexture->uv_mapping();
    }

    //-------------------------------------------------------------------------------------

    const unsigned TextureAtlas::width() const
    {
        return m_AtlasTexture->width();
    }

    //-------------------------------------------------------------------------------------

    const unsigned TextureAtlas::height() const
    {
        return m_AtlasTexture->height();
    }
    //-------------------------------------------------------------------------------------

    glm::uvec2 TextureAtlas::resolution() const
    {
        return m_AtlasTexture->resolution();
    }

    //-------------------------------------------------------------------------------------

    bool TextureAtlas::has_sub_texture(const CKey<std::string>& key) const
    {
        return m_SubTextureData.count(key);
    }
    
    //-------------------------------------------------------------------------------------

    std::vector<CKey<std::string>> TextureAtlas::get_sub_texture_keys() const
    {
        std::vector<CKey<std::string>> keys;
        keys.reserve(m_SubTextureData.size());
        for(const auto& [key, sub_texture_data] : m_SubTextureData)
        {
            keys.push_back(key);
        }
        return keys;
    }

    //-------------------------------------------------------------------------------------

    TextureAtlas::SubTextureData TextureAtlas::get_sub_texture_data(const CKey<std::string>& key) const
    {
        CBN_Assert(m_SubTextureData.count(key) == 1, "Key is not associated with any sub-textures");
        return m_SubTextureData.at(key);
    }
    //-------------------------------------------------------------------------------------

    void TextureAtlas::configure(const Texture::Properties& properties)
    {
        m_AtlasTexture->configure(properties);
    }

    //-------------------------------------------------------------------------------------

    Texture::Properties TextureAtlas::get_properties() const
    {
        return m_AtlasTexture->get_properties();
    }
    
    //-------------------------------------------------------------------------------------

    const SRes<Texture> TextureAtlas::as_texture() const
    {
        return m_AtlasTexture;
    }
    
    //-------------------------------------------------------------------------------------

}