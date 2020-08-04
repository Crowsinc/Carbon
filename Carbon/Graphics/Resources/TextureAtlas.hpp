#pragma once

#include <unordered_map>
#include <filesystem>
#include <optional>
#include <vector>

#include "../OpenGL/TextureParameters.hpp"
#include "../OpenGL/GLTypedObject.hpp"
#include "../../Memory/Resource.hpp"
#include "Image.hpp"

namespace cbn
{

	class TextureAtlas : public GLTypedObject<GL_TEXTURE, GL_TEXTURE_2D>
	{
	public:

		struct ImageReference
		{
			int index;
			bool rotated;
			std::string key;
			unsigned x, y;
			unsigned width, height;
		};

		static SRes<TextureAtlas> Pack(const unsigned max_width, const unsigned max_height, const std::vector<SRes<Image>>& images);

		static SRes<TextureAtlas> Open(const std::filesystem::path& path, Swizzle swizzle = Swizzle::RGBA);

	private:

		struct OpenSpace
		{
			unsigned x, y, width, height;
		};

		std::unordered_map<int, ImageReference> m_TexturesByID;
		std::unordered_map<std::string, int> m_TexturesByString;

		TextureAtlas(const Image& image, std::vector<ImageReference> references); 

	public:

	};

}