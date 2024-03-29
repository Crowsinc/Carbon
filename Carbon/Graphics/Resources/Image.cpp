#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace cbn
{
    //-------------------------------------------------------------------------------------

    SRes<Image> Image::Create(const unsigned width, const unsigned height, std::vector<Colour>& data)
    {
        // If the given data does not exactly match the resolution 
        // or the resolution makes no sense, fail image creation
        if(static_cast<unsigned>(width * height) != data.size() || width * height == 0)
            return nullptr;

        // If we are here, everything went fine so create the image
        return Resource::WrapShared(new Image(width, height, data.data(), false));
    }
   
    //-------------------------------------------------------------------------------------

    SRes<Image> Image::Create(const unsigned width, const unsigned height, std::vector<Colour>&& data)
    {
        // If the given data does not exactly match the resolution 
        // or the resolution makes no sense, fail image creation
        if(static_cast<unsigned>(width * height) != data.size() || width * height == 0)
            return nullptr;

        // If we are here, everything went fine so create the image 
        // Since we are moving the data, shrink the vector 
        // then take ownership of the internal pointer
        data.shrink_to_fit();
        return Resource::WrapShared(new Image(width, height, data.data(), true));
    }
    
    //-------------------------------------------------------------------------------------

    SRes<Image> Image::Create(const unsigned width, const unsigned height)
    {
        return Resource::WrapShared(new Image(width, height));
    }
    
    //-------------------------------------------------------------------------------------
    
    SRes<Image> Image::Open(const std::filesystem::path& path)
    {
        // Fail if the path does not lead to an existing file
        if(!std::filesystem::exists(path) && !std::filesystem::is_regular_file(path))
            return nullptr;

        // Load image using stb_image
        int width, height, components;
        stbi_uc* image_data = stbi_load(path.string().c_str(), &width, &height, &components, m_Components);
        if(image_data == NULL)
            return nullptr;

        // When we create the image, take ownership of the data
        // Note that stb_image expects you to call stbi_free on the
        // image_data, but we will instead deallocate it normally since
        // thats all it does in this version of stb_image and it will provide
        // a performance boost since we dont need to copy the data. 
        return Resource::WrapShared(new Image(width, height, reinterpret_cast<Colour*>(image_data), true));
    }

    //-------------------------------------------------------------------------------------
    
    void Image::insert_pixels(const unsigned x_offset, const unsigned y_offset, const Colour* pixels, const unsigned width, const unsigned height)
    {
        // Clip the pixels if they extend out of the image
        const unsigned end_x = std::min(x_offset + width, this->width());
        const unsigned end_y = std::min(y_offset + height, this->height());

        // Copy over all pixels to the image
        for(unsigned y = y_offset; y < end_y; y++)
        {
            for(unsigned x = x_offset; x < end_x; x++)
            {
                set_pixel(x, y, pixels[coord_to_index(x - x_offset, y - y_offset, width)]);
            }
        }
    }

    //-------------------------------------------------------------------------------------

    void Image::insert_pixels_rotated(const unsigned x_offset, const unsigned y_offset, const Colour* pixels, const unsigned width, const unsigned height)
    {
        // Clip the pixels if they extend out of the image
        // Note that we are rotating the pixels, so we compare the 
        // height of the pixels with the width of this image vice-versa. 
        const unsigned end_x = std::min(x_offset + height, this->width());
        const unsigned end_y = std::min(y_offset + width, this->height());

        // Copy over all pixels to the image
        // Note that we are retrieving values from the pixels
        // array with a switched x and y coord, so to avoid 
        // cache misses, we actually want to loop x first. 
        for(unsigned x = x_offset; x < end_x; x++)
        {
            for(unsigned y = y_offset; y < end_y; y++)
            {
                // The image is being inserted with a rotation, so the x and y
                // are swapped between the coordinated of this image and the pixels
                // being inserted. 
                set_pixel(x, y, pixels[coord_to_index(y - y_offset, x - x_offset, width)]);
            }
        }
    }
    
    //-------------------------------------------------------------------------------------

    void Image::allocate(const unsigned width, const unsigned height, bool set_to_zero)
    {
        if(set_to_zero)
            m_Data = std::unique_ptr<Colour>(new Colour[static_cast<uint64_t>(width) * height]);
        else
            m_Data = std::unique_ptr<Colour>(new Colour[static_cast<uint64_t>(width) * height]);
    }
 
    //-------------------------------------------------------------------------------------

    unsigned Image::coord_to_index(const unsigned x, const unsigned y) const
    {
        return y * width() + x;
    }
    //-------------------------------------------------------------------------------------

    unsigned Image::coord_to_index(const unsigned x, const unsigned y, const unsigned width) const
    {
        return y * width + x;
    }

    //-------------------------------------------------------------------------------------

    Image::Image(const unsigned width, const unsigned height, Colour* data, bool take_ownership)
        : m_Resolution(width, height)
    {
        // If we are taking ownership of the data, simply wrap it in a unique pointer and use it
        // Otherwise, create a backing array and copy the data over. 
        if(take_ownership)
        {
            m_Data = std::unique_ptr<Colour>(data);
        }
        else
        {
            allocate(width, height, false);
            insert_pixels(0, 0, data, width, height);
        }
    }
    
    //-------------------------------------------------------------------------------------
    
    Image::Image(const unsigned width, const unsigned height)
        : m_Resolution(width, height)
    { 
        allocate(width, height, true);
    }
    
    //-------------------------------------------------------------------------------------
    
    void Image::fill(const Colour& colour)
    {
        // Replace all pixels with the given colour
        for(unsigned i = 0; i < size(); i++)
        {
            *(m_Data.get() + i) = colour;
        }
    }
    
    //-------------------------------------------------------------------------------------

    void Image::fill(const SRes<Image>& image)
    {
        // Re-use the insert pixels method to insert the image
        // If the image is smaller than this image, tile it
        for(unsigned y = 0; y < height(); y += image->height())
        {
            for(unsigned x = 0; x < width(); x += image->width())
            {
                insert_pixels(x, y, image->m_Data.get(), image->width(), image->height());
            }
        }
    }
    
    //-------------------------------------------------------------------------------------

    void Image::insert(const unsigned x, const unsigned y, const SRes<Image>& image, const bool rotate)
    {
        if(rotate)
            insert_pixels_rotated(x, y, image->m_Data.get(), image->width(), image->height());
        else
            insert_pixels(x, y, image->m_Data.get(), image->width(), image->height());
    }
    
    //-------------------------------------------------------------------------------------

    void Image::set_pixel(const unsigned x, const unsigned y, const Colour& pixel)
    {
        CBN_Assert(x < width() && y < height(), "Coordinate out of bounds");

        *(m_Data.get() + coord_to_index(x, y)) = pixel;
    }
    
    //-------------------------------------------------------------------------------------
    
    Colour Image::get_pixel(const unsigned x, const unsigned y) const
    {
        CBN_Assert(x < width() && y < height(), "Coordinate out of bounds");

        return *(m_Data.get() + coord_to_index(x, y));
    }
    
    //-------------------------------------------------------------------------------------
    
    bool Image::save(const std::filesystem::path& path) const
    {
        // Use stb_image_write to save the image to a png file
        return stbi_write_png(path.string().c_str(), width(), height(), m_Components, m_Data.get(), sizeof(Colour) * width());
    }
    
    //-------------------------------------------------------------------------------------
    
    const unsigned Image::width() const
    {
        return m_Resolution.x;
    }
    
    //-------------------------------------------------------------------------------------
    
    const unsigned Image::height() const
    {
        return m_Resolution.y;
    }
    
    //-------------------------------------------------------------------------------------
    
    glm::uvec2 Image::resolution() const
    {
        return m_Resolution;
    }

    //-------------------------------------------------------------------------------------

    uint64_t Image::byte_size() const
    {
        return static_cast<uint64_t>(m_Resolution.x) * m_Resolution.y * sizeof(Colour);
    }

    //-------------------------------------------------------------------------------------

    unsigned Image::size() const
    {
        return m_Resolution.x * m_Resolution.y;
    }
    
    //-------------------------------------------------------------------------------------

    int Image::components() const
    {
        return m_Components;
    }
    
    //-------------------------------------------------------------------------------------

    const Colour* Image::data() const
    {
        return m_Data.get();
    }
    
    //-------------------------------------------------------------------------------------
    
    Colour& Image::operator()(const unsigned x, const unsigned y)
    {
        CBN_Assert(x < width() && y < height(), "Coordinate out of bounds");
    
        return *(m_Data.get() + coord_to_index(x, y));
    }

    //-------------------------------------------------------------------------------------
 
}