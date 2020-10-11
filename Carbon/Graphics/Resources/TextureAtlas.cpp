#include "TextureAtlas.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>

#include "../../Maths/Maths.hpp"
#include "Texture.hpp"

namespace cbn
{
    
    //-------------------------------------------------------------------------------------

    TextureUVMap TextureAtlas::calculate_uvs(const Rect<int>& rect, const bool rotated, const glm::vec2& atlas_resolution)
    {
        const glm::vec2 top_left_uv = glm::vec2{rect.x, rect.y} / atlas_resolution;
        const glm::vec2 bot_left_uv = glm::vec2{rect.x, rect.y} / atlas_resolution;
        const glm::vec2 bot_right_uv = glm::vec2{rect.x + rect.width, rect.y + rect.height} / atlas_resolution;
        const glm::vec2 top_right_uv = glm::vec2{rect.x + rect.width,  rect.y} / atlas_resolution;

        
        // If the texture is rotated, we need to counter rotate the uvs by 90 degrees
        if(rotated)
        {
            return {
                bot_left_uv,
                bot_right_uv,
                top_right_uv,
                top_left_uv
            };
        }
        else
        {
            return {
                top_left_uv,
                bot_left_uv,
                bot_right_uv,
                top_right_uv
            };
        }

    }
    
    //-------------------------------------------------------------------------------------

    bool TextureAtlas::is_rotated(const Rect<int>& rect, const SRes<Image>& image)
    {
        return rect.width != rect.height && rect.width == image->height() && rect.height == image->width();
    }
    
    //-------------------------------------------------------------------------------------

    glm::uvec2 TextureAtlas::determine_footprint(const std::vector<Rect<int>>& rectangles)
    {
        const auto rightmost_rect = std::max_element(rectangles.begin(), rectangles.end(), [](const Rect<int> left, const Rect<int> right)
        {
            return left.x + left.width < right.x + right.width;
        });

        const auto leftmost_rect = std::max_element(rectangles.begin(), rectangles.end(), [](const Rect<int> left, const Rect<int> right)
        {
            return left.y + left.height < right.y + right.height;
        });

        return {rightmost_rect->x + rightmost_rect->width, leftmost_rect->y + leftmost_rect->height};
    }

    //-------------------------------------------------------------------------------------
    
    SRes<TextureAtlas> TextureAtlas::Pack(const unsigned width, const unsigned height, const std::unordered_map<Name, SRes<Image>>& images, const TexturePackingSettings settings)
    {
        // Create rectangles representing the images, these will be used in a rectangle packing algorithm to represent the textures. 
        // The rects in the rectangles vector are in the same order as those in the images unordered map when iterated. 
        std::vector<Rect<int>> rectangles(images.size());
        std::transform(images.begin(), images.end(), rectangles.begin(), [&](const std::pair<Name, SRes<Image>>& pair)
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
            std::vector<SubTexture> subtextures;
            for(auto const& [reference_name, image] : images)
            {
                const auto& rect = rectangles[i++];
                const bool rotated = is_rotated(rect, image);

                const SubTexture subtexture = {
                    rotated,
                    calculate_uvs(rect, rotated, atlas_image->resolution()),
                    {rect.x, rect.y},
                    {rect.width, rect.height},
                    reference_name
                };

                subtextures.push_back(subtexture);
                atlas_image->insert(rect.x, rect.y, image, rotated);
            }

            // Create the texture
            TextureSettings properties;
            properties.minifying_filter = TextureFilter::NEAREST;
            properties.magnifying_filter = TextureFilter::NEAREST;
            properties.vertical_wrapping = TextureWrapping::CLAMP_TO_EDGE;
            properties.horizontal_wrapping = TextureWrapping::CLAMP_TO_EDGE;
            SRes<Texture> atlas_texture = Texture::Create(atlas_image, properties);
            
            if(!atlas_texture)
                return nullptr;

            return Resource::WrapShared<TextureAtlas>(new TextureAtlas(atlas_texture, subtextures));
        }
        return nullptr;
    }
    
    //-------------------------------------------------------------------------------------
   
    TextureAtlas::TextureAtlas(const SRes<Texture>& texture, const std::vector<SubTexture>& subtextures)
        : m_AtlasTexture(texture),
        m_SubTextures(subtextures) 
    {
        for(int i = 0; i < m_SubTextures.size(); i++)
            m_SubTextureMap[m_SubTextures[i].texture_name] = i;
    }
    
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

    TextureUVMap TextureAtlas::uvs() const
    {
        return m_AtlasTexture->uvs();
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

    bool TextureAtlas::has_subtexture(const Name& reference_name) const
    {
        return m_SubTextureMap.count(reference_name);
    }

    //-------------------------------------------------------------------------------------

    SubTexture TextureAtlas::get_subtexture(const Name& reference_name) const
    {
        CBN_Assert(has_subtexture(reference_name), "Reference name does not belong to a sub-texture in this atlas");

        return m_SubTextures[m_SubTextureMap.at(reference_name)];
    }

    //-------------------------------------------------------------------------------------

    const std::vector<SubTexture>& TextureAtlas::subtextures() const
    {
        return m_SubTextures;
    }

    //-------------------------------------------------------------------------------------

    void TextureAtlas::configure(const TextureSettings& settings)
    {
        m_AtlasTexture->configure(settings);
    }

    //-------------------------------------------------------------------------------------

    TextureSettings TextureAtlas::settings() const
    {
        return m_AtlasTexture->settings();
    }
    
    //-------------------------------------------------------------------------------------

    SRes<Texture> TextureAtlas::as_texture() const
    {
        return m_AtlasTexture;
    }
    
    //-------------------------------------------------------------------------------------

}