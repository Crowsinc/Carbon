#pragma once

#include <stdint.h>
#include <variant>

#include "OpenGL/VertexArrayObject.hpp"
#include "Resources/ShaderProgram.hpp"
#include "Resources/StaticBuffer.hpp"
#include "Resources/StreamBuffer.hpp"
#include "Animation/Animation.hpp"
#include "../Maths/Transform.hpp"
#include "../Utility/Version.hpp"
#include "TexturePack.hpp"
#include "Camera.hpp"

namespace cbn
{

	//TODO: implement properly as standalone class
	struct BoundingBox
	{
		Transform transform = {0,0};
		glm::vec2 size = {32,32};

		//TODO: 
		/*
			- AABB & circle bounding boxes
			- collision etc.
			- grabbing of vertices
			- transforms

			ALSO MAKE A BOUNDING CIRCLE CLASS
				so for collision we can handle both boxes and circles

		*/
	};

	struct SpriteRendererProperties
	{
		uint16_t sprites_per_batch = 16384;
		uint32_t buffer_allocation_bias = 3;
	};

	struct SpriteRendererSettings
	{
		bool cull_outside_camera = true;
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
		SpriteRendererSettings m_Settings;
		glm::mat4 m_ViewProjectionMatrix;
		BoundingBox m_CameraBoundingBox;
		TexturePack m_TexturePack;

		void initialize_renderer(const Version& opengl_version);

		bool is_sprite_visible(const BoundingBox& sprite);

		void push_sprite_to_buffer(const BoundingBox& sprite, const uint16_t index_1, const uint16_t index_2, const uint16_t index_3, const uint16_t index_4, const glm::uvec4& vertex_data);

	public:

		SpriteRenderer(const Version& opengl_version, const SpriteRendererSettings& settings = {}, const SpriteRendererProperties& properties = {});

		void begin_batch(const Camera& camera);

		void submit(const BoundingBox& sprite);
		
		void submit(const BoundingBox& sprite, const glm::uvec4& vertex_data = c_EmptyVertexData);
		
		void submit(const BoundingBox& sprite, const Name& texture_name_1, const glm::uvec4& vertex_data = c_EmptyVertexData);
		
		void submit(const BoundingBox& sprite, const Name& texture_name_1, const Name& texture_name_2, const glm::uvec4& vertex_data = c_EmptyVertexData);
		
		void submit(const BoundingBox& sprite, const Name& texture_name_1, const Name& texture_name_2, const Name& texture_name_3, const glm::uvec4& vertex_data = c_EmptyVertexData);
		
		void submit(const BoundingBox& sprite, const Name& texture_name_1, const Name& texture_name_2, const Name& texture_name_3, const Name& texture_name_4, const glm::uvec4& vertex_data = c_EmptyVertexData);

		void end_batch();

		void render(const SRes<ShaderProgram>& shader);

		bool is_batch_started() const;

		bool is_batch_full() const;

		int batch_size() const;

		SpriteRendererSettings settings() const;

		SpriteRendererProperties properties() const;

		void configure(const SpriteRendererSettings& settings);

		void set_texture_pack(const TexturePack& textures);

	};
}
