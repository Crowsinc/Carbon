#include "TextureAtlas.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>

#include "Texture.hpp"

namespace cbn
{

    //-------------------------------------------------------------------------------------
    
    SRes<TextureAtlas> TextureAtlas::Pack(const unsigned width, const unsigned height, const PackingSettings settings, const std::vector<SRes<Image>> images)
    {
        std::vector<Rect<int>> rectangles;

        for(auto image : images)
        {
            rectangles.push_back({0,0, (int)image->width(), (int)image->height()});
        }

        if(max_rects_optimal({width, height}, settings.allow_image_rotation, settings.heuristic, rectangles)){
            auto atlas = Image::Create(width, height);

            for(int i = 0; i < images.size(); i++)
            {
                const auto& rect = rectangles[i];

                atlas->insert(rect.x, rect.y, images[i], rect.width != rect.height && rect.width == images[i]->height() && rect.height == images[i]->width());
            }

            atlas->save("Atlas.png");
        }
        else std::cout << "Packing Failed!" << std::endl;

        return nullptr;
    }
    //-------------------------------------------------------------------------------------

}