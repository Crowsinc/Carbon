#pragma once

#include <glm/glm.hpp>
#include <filesystem>
#include <vector>

#include "../../Memory/Resource.hpp"

namespace cbn
{

	class Image
	{
	public:

#pragma pack(push, 1)

		union Pixel
		{
			unsigned data;

			struct
			{
				uint8_t red;
				uint8_t green;
				uint8_t blue;
				uint8_t alpha;
			};

			void operator=(const Pixel& other);
		};

#pragma pack(pop)

		static SRes<Image> Create(const unsigned width, const unsigned height, std::vector<Pixel>& data);

		static SRes<Image> Create(const unsigned width, const unsigned height, std::vector<Pixel>&& data);

		static SRes<Image> Create(const unsigned width, const unsigned height);

		static SRes<Image> Open(const std::filesystem::path& path, const bool flip_vertically = false);

	private:

		static constexpr int m_Components = 4;

		std::shared_ptr<Pixel> m_Data;
		glm::uvec2 m_Resolution;

		void insert_pixels(const unsigned x_offset, const unsigned y_offset, const Pixel* pixels, const unsigned width, const unsigned height);

		void insert_pixels_rotated(const unsigned x_offset, const unsigned y_offset, const Pixel* pixels, const unsigned width, const unsigned height);

		void allocate(const unsigned width, const unsigned height, bool set_to_zero);

		unsigned coord_to_index(const unsigned x, const unsigned y) const;

		unsigned coord_to_index(const unsigned x, const unsigned y, const unsigned width) const;

		Image(const unsigned width, const unsigned height, Pixel* data, const bool take_ownership);

		Image(const unsigned width, const unsigned height);

	public:

		void fill(const Pixel& color);

		void fill(const Image& image);

		void insert(const unsigned x, const unsigned y, const Image& image, const bool rotate_90_degrees = false);
		
		void set_pixel(const unsigned x, const unsigned y, const Pixel& pixel);

		Pixel get_pixel(const unsigned x, const unsigned y) const;

		bool save(const std::filesystem::path& path) const;

		const unsigned width() const;

		const unsigned height() const;

		glm::uvec2 resolution() const;

		unsigned size() const;

		int components() const;
		
		const Pixel* data() const;

		Pixel& operator()(const unsigned x, const unsigned y);

	};

}