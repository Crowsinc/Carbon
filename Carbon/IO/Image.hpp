#pragma once

#include <vector>

namespace cbn
{

	class Image
	{
	public:

		enum class Format
		{
			RGBA32,
			RGB24,
			RGBA25
		};


	private:


	public:


		Image(const int width, const int height, const Format format, const std::vector<uint8_t> data);
		
		Image(const int width, const int height, const Format format);

	};


}