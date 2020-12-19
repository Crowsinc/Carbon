#pragma once

#include <vector>
#include <variant>
#include <unordered_map>

#include "../Data/Identity/Identifier.hpp"
#include "Resources/ShaderProgram.hpp"
#include "Resources/BufferTexture.hpp"
#include "Resources/TextureAtlas.hpp"
#include "Resources/Texture.hpp"
#include "../Data/String.hpp"

namespace cbn
{
	using TexturePackReference = std::variant<std::monostate, SRes<Texture>, SRes<TextureAtlas>>;
	
	struct TexturePackEntry
	{
		Identifier identifier;
		TexturePackReference texture;
	};

	class TexturePack
	{
	public:

		static constexpr uint64_t SupportedTextureCount = 15;

	private:

#pragma pack(push, 1)

		struct DataLayout
		{
			glm::vec3 uv_data_1;
			glm::vec3 uv_data_2;
			glm::vec3 uv_data_3;
			glm::vec3 uv_data_4;
		};

#pragma pack(pop)

		bool m_Empty;
		Version m_OpenGLVersion;
		SRes<BufferTexture> m_BufferTexture;

		std::vector<String> m_TextureNames;
		std::vector<SRes<Texture>> m_Textures;
		std::vector<TextureUVMap> m_TextureUVs;
		
		std::unordered_map<Identifier, uint32_t> m_UVLookupMap;
		std::unordered_map<Identifier, uint32_t> m_TextureLookupMap;
		std::unordered_map<Identifier, uint32_t> m_DataIndexLookupMap;

		DataLayout pack_data(const TextureUVMap& uvs, const uint64_t texture_index);

		void initialize(const std::array<TexturePackEntry, SupportedTextureCount>& textures);

	public:

		TexturePack(const Version& opengl_version);

		TexturePack(const TexturePack& other);

		TexturePack(const std::array<TexturePackEntry, SupportedTextureCount>& textures, const Version& opengl_version);
		
		void operator=(const std::array<TexturePackEntry, SupportedTextureCount>& textures);

		void operator=(const TexturePack& other);
		
		const SRes<Texture> texture_of(const Identifier& texture_identifier) const;

		const TextureUVMap uvs_of(const Identifier& texture_identifier) const;

		unsigned position_of(const Identifier& texture_identifier) const;

		bool contains(const Identifier& texture_identifier) const;

		const std::vector<SRes<Texture>> textures() const;

		const std::vector<String> texture_names() const;
		
		const std::vector<TextureUVMap>& uvs() const;
		
		int texture_resource_count() const;

		int texture_count() const;
		
		bool is_empty() const;

		bool is_bound() const;

		void unbind() const;

		void bind() const;

	};
}
