#pragma once

#include <unordered_map>
#include <filesystem>
#include <optional>
#include <vector>

#include "../../Algorithms/BinPacking.hpp"
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
			unsigned x, y;
			unsigned width, height;
		};

		struct PackingSettings
		{
			BinPackingHeuristic heuristic = BinPackingHeuristic::BLSF_BAF_SQR;
			bool allow_image_rotation = true;
			bool shrink_to_footprint = true;
		};


		static SRes<TextureAtlas> Pack(const unsigned width, const unsigned height, const PackingSettings settings, const std::vector<SRes<Image>> images);

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