#pragma once

#include <glm/glm.hpp>
#include <filesystem>
#include <vector>

#include "../../Data/Identity/Identifiable.hpp"
#include "../../Memory/Resource.hpp"
#include "../../Utility/Colour.hpp"

namespace cbn
{


	class Image : public Identifiable
	{
	public:

		static SRes<Image> Create(const unsigned width, const unsigned height, std::vector<Colour>& data);

		static SRes<Image> Create(const unsigned width, const unsigned height, std::vector<Colour>&& data);

		static SRes<Image> Create(const unsigned width, const unsigned height);

		static SRes<Image> Open(const std::filesystem::path& path);

	private:

		static constexpr int m_Components = 4;

		std::shared_ptr<Colour> m_Data;
		glm::uvec2 m_Resolution;

		void insert_pixels(const unsigned x_offset, const unsigned y_offset, const Colour* pixels, const unsigned width, const unsigned height);

		void insert_pixels_rotated(const unsigned x_offset, const unsigned y_offset, const Colour* pixels, const unsigned width, const unsigned height);

		void allocate(const unsigned width, const unsigned height, bool set_to_zero);

		unsigned coord_to_index(const unsigned x, const unsigned y) const;

		unsigned coord_to_index(const unsigned x, const unsigned y, const unsigned width) const;

		Image(const unsigned width, const unsigned height, Colour* data, const bool take_ownership);

		Image(const unsigned width, const unsigned height);

	public:

		void fill(const Colour& colour);

		void fill(const SRes<Image>& image);

		void insert(const unsigned x, const unsigned y, const SRes<Image>& image, const bool rotate_90_degrees = false);
		
		void set_pixel(const unsigned x, const unsigned y, const Colour& pixel);

		Colour get_pixel(const unsigned x, const unsigned y) const;

		//TODO: draw(x, y, shape, colour)

		bool save(const std::filesystem::path& path) const;

		const unsigned width() const;

		const unsigned height() const;

		glm::uvec2 resolution() const;

		uint64_t byte_size() const;

		unsigned size() const;

		int components() const;
		
		const Colour* data() const;

		Colour& operator()(const unsigned x, const unsigned y);

	};

}