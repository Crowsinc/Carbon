#pragma once

#include <stdint.h>
#include <variant>

#include "../Data/Identity/Identifier.hpp"
#include "OpenGL/VertexArrayObject.hpp"
#include "Resources/ShaderProgram.hpp"
#include "Resources/StaticBuffer.hpp"
#include "Resources/StreamBuffer.hpp"
#include "../Utility/Version.hpp"
#include "TexturePack.hpp"
#include "Camera.hpp"

namespace cbn
{

	//TODO: improve user data situation

	struct SpriteRendererProperties
	{
		uint16_t sprites_per_batch = 4096;
		uint32_t buffer_allocation_bias = 32;
	};

	class SpriteRenderer
	{
	private:

#pragma pack(push, 1)

		struct VertexLayout
		{
			glm::vec2 position;
			uint16_t texture[4];
			glm::uvec4 data;
		};

		struct SpriteLayout
		{
			VertexLayout vertex_1;
			VertexLayout vertex_2;
			VertexLayout vertex_3;
			VertexLayout vertex_4;
		};

#pragma pack(pop)

		static constexpr uint32_t c_IndicesPerSprite = 6;
		static constexpr uint32_t c_VerticesPerSprite = 4;
		static constexpr glm::uvec4 c_EmptyVertexData = {0,0,0,0};

		SRes<StreamBuffer> m_StreamBuffer;
		SRes<StaticBuffer> m_IndexBuffer;
		VertexArrayObject m_VertexArray;
		SpriteLayout* m_BufferPtr;

		bool m_BatchStarted, m_BatchEnded;
		uint64_t m_SpritesPerStreamBuffer;
		uint64_t m_BatchStartPosition;
		uint64_t m_BatchEndPosition;
		uint32_t m_CurrentBatchSize;

		const SpriteRendererProperties m_Properties;
		glm::mat4 m_ViewProjectionMatrix;
		TexturePack m_TexturePack;

		void initialize_renderer(const Version& opengl_version);

		void push_sprite_to_buffer(const StaticMesh<4>& quad, const uint16_t& index_1, const uint16_t& index_2, const uint16_t& index_3, const uint16_t& index_4, const glm::uvec4& vertex_data);

	public:

		SpriteRenderer(const Version& opengl_version, const SpriteRendererProperties& properties = {});

		void begin_batch(const Camera& camera);

		void submit(const StaticMesh<4>& quad);
		void submit(const StaticMesh<4>& quad, const glm::uvec4& vertex_data);
		void submit(const StaticMesh<4>& quad, const Identifier& texture_1);
		void submit(const StaticMesh<4>& quad, const Identifier& texture_1, const glm::uvec4& vertex_data);
		void submit(const StaticMesh<4>& quad, const Identifier& texture_1, const Identifier& texture_2);
		void submit(const StaticMesh<4>& quad, const Identifier& texture_1, const Identifier& texture_2, const glm::uvec4& vertex_data);
		void submit(const StaticMesh<4>& quad, const Identifier& texture_1, const Identifier& texture_2, const Identifier& texture_3);
		void submit(const StaticMesh<4>& quad, const Identifier& texture_1, const Identifier& texture_2, const Identifier& texture_3, const glm::uvec4& vertex_data);
		void submit(const StaticMesh<4>& quad, const Identifier& texture_1, const Identifier& texture_2, const Identifier& texture_3, const Identifier& texture_4);
		void submit(const StaticMesh<4>& quad, const Identifier& texture_1, const Identifier& texture_2, const Identifier& texture_3, const Identifier& texture_4, const glm::uvec4& vertex_data);

		void end_batch();

		void render(const SRes<ShaderProgram>& shader);

		bool is_batch_started() const;

		bool is_batch_full() const;

		int batch_size() const;

		SpriteRendererProperties properties() const;

		void set_texture_pack(const TexturePack& textures);

	};
}
