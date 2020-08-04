#include "TextureAtlas.hpp"

#include <algorithm>
#include <numeric>

#include "../../Utility/Algorithms.hpp"

#include "Texture.hpp"

#include <iostream>

namespace cbn
{

    //-------------------------------------------------------------------------------------
    
    SRes<TextureAtlas> TextureAtlas::Pack(const unsigned max_width, const unsigned max_height, const std::vector<SRes<Image>>& images)
    {
        //TODO: rewrite this but properly using methods.
        // Also try figure out why it cant fit everything when sorting by height
        // I imagine it has to do with it not attempting to place the image at every y

        std::vector<unsigned> widths(max_height);
    
        // Since we cannot modify the contents of the images vector
        // we will instead use a vector of indices to modify the order
        // of the images as required. To do this, first fill the vector 
        // with all the indices of the images. 
        std::vector<int> image_indices(images.size());
        std::iota(image_indices.begin(), image_indices.end(), 0);
    
        // Since we are going to pack the images horizontally, we want
        // to sort them by their height so that images of the same height
        // will be next to each other. This will minimise wasted space from
        // internal fragmentation and external fragmentation. 
        std::sort(image_indices.begin(), image_indices.end(), [&](const int i, const int j) -> bool
        {
            return images[i]->height() < images[j]->height();
        });
    
        unsigned max_x = 0, max_y = 0;
        std::vector<ImageReference> image_spaces;
    

        for(int i = 0; i < image_indices.size(); i++)
        {
            const Image& image = *images[image_indices[i]];
            bool inserted = false;

            // Search for a spot the size of the height of the image
            // which can fit the width of the image in the atlas, and
            // the width does not vary. 
            // (NOTE MAYBE LATER TRY WITH VARYING WIDTH (INTERNAL FRAG)
            bool potential_spot = false;
            unsigned spot_started_index = 0;
            int spot_started_leftover_space = 0;
            int curr_height = 0;
            unsigned curr_width = 0;
           
            for(int y = 0; y < max_height; y++)
            {
                const unsigned width = widths[y];
                const unsigned leftover_space = max_width - width;
    
                if(leftover_space >= image.width())
                {
                    if(!potential_spot || (potential_spot && leftover_space > spot_started_leftover_space))
                    {
                        spot_started_index = y;
                        spot_started_leftover_space = leftover_space;
                        potential_spot = true;
                        curr_height = 1;
                        curr_width = width;
                    }
                    // Note that this third condition is already taken care of by 
                    // the outer if statement but ill put it here for clarity
                    else if(potential_spot && leftover_space < spot_started_leftover_space && leftover_space >= image.width())
                    {
                        // This is ran if we can fit something in by shifting it right, saving vertical space but
                        // adding internal fragmentation as a result. Wether this is a good thing or not will 
                        // depend on whether anything could fit in there anyway. Maybe we can track the smallest
                        // dimension and check whether its worth it or not this way. Note that we only do this
                        // if the width at this height is smaller than the leftover space when we started,
                        // because otherwise it would be better to move to a new row or we would block images
                        // from being inserted left of this one. 
                        curr_height++;
                        spot_started_leftover_space = leftover_space;
                        curr_width = width;
                    
                        // When we do this, it may be possible to a texture up at the new width
                        // Note that we can only do this because our insertion method guarantees
                        // that there will not be any overhand, so if one top pixel of the image
                        // collides, we can guarantee that the others will collide too. 
                        while(spot_started_index >= 1 && widths[spot_started_index - 1] <= curr_width)
                        {
                            curr_height++;
                            spot_started_index--;
                        }
                    }
                    else if(potential_spot && spot_started_leftover_space == leftover_space) 
                    {
                        curr_height++;
                    }
                    else
                    {
                        potential_spot = false;
                        y--;
                        continue;
                    }

                    // If the potential spot fits the height of the image
                    // then it must also fit the width, so insert it and
                    // update the widths and heights as required. 
                    if(potential_spot && curr_height == image.height())
                    {
                        // insert the image
                        if(curr_width + image.width() > max_x)
                            max_x = curr_width + image.width();

                        if(y > max_y)
                            max_y = y;

                        image_spaces.push_back(ImageReference{image_indices[i],false, "", curr_width, spot_started_index, image.width(), image.height()});

                        // Update the widths
                        for(int k = spot_started_index; k < spot_started_index + image.height(); k++)
                        {
                            widths[k] = curr_width + image.width();
                        }

                        inserted = true;
                        break;
                    } 
                }
                else potential_spot = false;
            }

            if(inserted == false)
            {
                std::cout << "could not fit all images";
            }
        }
    
        SRes<Image> atlas = Image::Create(max_x, max_y);
    

        for(int i = 0; i < image_spaces.size(); i++)
        {
            auto references = image_spaces[i];
    
            atlas->insert(references.x, references.y, *images[image_indices[i]], false);
        }
    
        atlas->save("NEW_atlas_image+IF+G.png");
    
        return nullptr;
    }

    //-------------------------------------------------------------------------------------

}