#pragma once

#include <glm/glm.hpp>
#include <filesystem>
#include <vector>

#include "../../Utility/Resource.hpp"

namespace cbn
{

	class Image
	{
	public:

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

		Res<Image> Create(const unsigned width, const unsigned height, std::vector<Pixel>& data);

		Res<Image> Create(const unsigned width, const unsigned height, std::vector<Pixel>&& data);

		Res<Image> Create(const unsigned width, const unsigned height);

		Res<Image> Open(const std::filesystem::path& path);

	private:

		static constexpr int m_Components = 4;

		std::unique_ptr<Pixel> m_Data;
		glm::uvec2 m_Resolution;

		void insert_pixels(const unsigned x_offset, const unsigned y_offset, const Pixel* pixels, const unsigned width, const unsigned height);

		void allocate(const unsigned width, const unsigned height, bool set_to_zero);

		unsigned coord_to_index(const unsigned x, const unsigned y) const;

		Image(const unsigned width, const unsigned height, Pixel* data, const bool take_ownership);

		Image(const unsigned width, const unsigned height);

	public:

		Image(Image&& image) noexcept;

		Image(const Image& image) = delete;

		void fill(const Pixel& color);

		void fill(const Image& image);

		void insert(const unsigned x, const unsigned y, const Image& image);
		
		void set_pixel(const unsigned x, const unsigned y, const Pixel& pixel);

		Pixel get_pixel(const unsigned x, const unsigned y) const;

		bool save(const std::filesystem::path& path) const;

		const unsigned width() const;

		const unsigned height() const;

		glm::uvec2 resolution() const;

		unsigned size() const;

		int components() const;
		
		Pixel& operator()(const unsigned x, const unsigned y);

		void operator=(Image&& image) noexcept;

	};

}