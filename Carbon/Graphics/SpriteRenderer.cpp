#include "SpriteRenderer.hpp"

namespace cbn
{
	
	//-------------------------------------------------------------------------------------
	
	void SpriteRenderer::initialize_renderer(const Version& opengl_version)
	{
		const auto index_buffer_indices = m_SpritesPerStreamBuffer * c_IndicesPerSprite;

		// Create batch indices for the entire stream buffer+
		std::vector<uint32_t> batch_indices;
		batch_indices.reserve(index_buffer_indices);
		for(auto index = 0, base_sprite_index = 0; index < index_buffer_indices; index += c_IndicesPerSprite, base_sprite_index += c_VerticesPerSprite)
		{
			batch_indices.push_back(base_sprite_index + 0);
			batch_indices.push_back(base_sprite_index + 1);
			batch_indices.push_back(base_sprite_index + 3);
			batch_indices.push_back(base_sprite_index + 3);
			batch_indices.push_back(base_sprite_index + 1);
			batch_indices.push_back(base_sprite_index + 2);
		}

		// Allocate the index buffer which will be shared for all batches
		m_IndexBuffer = StaticBuffer::Allocate(reinterpret_cast<uint8_t*>(batch_indices.data()), batch_indices.size() * sizeof(uint32_t), BufferTarget::ELEMENT_BUFFER, opengl_version);
		CBN_Assert(m_IndexBuffer != nullptr, "Index buffer creation failed");

		// Allocate the stream buffer which will stream the sprite data to the shaders
		m_StreamBuffer = StreamBuffer::Allocate(BufferTarget::VERTEX_BUFFER, m_SpritesPerStreamBuffer * sizeof(SpriteLayout), false, opengl_version);
		CBN_Assert(m_StreamBuffer != nullptr, "Stream buffer creation failed");

		// Set up the vertex array
		m_VertexArray.bind();

		// Bind the buffers to the vertex array
		m_StreamBuffer->force_bind();
		m_IndexBuffer->force_bind();

		// Set attribute bindings for the stream buffer and sprite layout
		int attribute = 0;
		glVertexAttribPointer(attribute++, 2, GL_FLOAT, false, sizeof(VertexLayout), (void*)offsetof(VertexLayout, position));
		glVertexAttribIPointer(attribute++, 4, GL_UNSIGNED_SHORT, sizeof(VertexLayout), (void*)offsetof(VertexLayout, texture));
		glVertexAttribIPointer(attribute++, 4, GL_UNSIGNED_INT, sizeof(VertexLayout), (void*)offsetof(VertexLayout, data));

		for(auto i = 0; i < attribute; i++)
			glEnableVertexAttribArray(i);

	}

	//-------------------------------------------------------------------------------------

	void SpriteRenderer::push_sprite_to_buffer(const BoundingBox& sprite, const uint16_t index_1, const uint16_t index_2, const uint16_t index_3, const uint16_t index_4, const glm::uvec4& vertex_data)
	{
		CBN_Assert(m_BatchStarted, "No batch exists for submission");
		CBN_Assert(!is_batch_full(), "Batch is full");

		const glm::mat4 mvp_matrix = build_mvp_matrix(m_ViewProjectionMatrix, sprite.transform.to_transform_matrix());

		// Determine the positions of the sprite's vertices in local space. We want the local
		// position of the verticies to be such that the origin is in the middle so that
		// the rotations applied by the transform matrix will be around the middle of the sprite. 
		// Since the origin is in the middle, we only need to specify the local position of two adjacent
		// vertices because the opposite position can be obtained from the negative of the position vector.
		const glm::vec2 half_size = sprite.size / 2.0f;
		const glm::vec2 vertex_0_local_position = {-half_size.x, half_size.y};
		const glm::vec2 vertex_1_local_position = {-half_size.x, -half_size.y};

		// Perform the transforms on the local space vertices to get their final positions for OpenGL. 
		const glm::vec2 vertex_1_position = transform(vertex_0_local_position, mvp_matrix);
		const glm::vec2 vertex_2_position = transform(vertex_1_local_position, mvp_matrix);
		const glm::vec2 vertex_3_position = transform(-vertex_0_local_position, mvp_matrix);
		const glm::vec2 vertex_4_position = transform(-vertex_1_local_position, mvp_matrix);

		// Set top left vertex data
		m_BufferPtr->vertex_1.position = vertex_1_position;
		m_BufferPtr->vertex_1.texture[0] = index_1 + 0;
		m_BufferPtr->vertex_1.texture[1] = index_2 + 0;
		m_BufferPtr->vertex_1.texture[2] = index_3 + 0;
		m_BufferPtr->vertex_1.texture[3] = index_4 + 0;
		m_BufferPtr->vertex_1.data = vertex_data;


		// Set bottom left vertex data
		m_BufferPtr->vertex_2.position = vertex_2_position;
		m_BufferPtr->vertex_2.texture[0] = index_1 + 1;
		m_BufferPtr->vertex_2.texture[1] = index_2 + 1;
		m_BufferPtr->vertex_2.texture[2] = index_3 + 1;
		m_BufferPtr->vertex_2.texture[3] = index_4 + 1;
		m_BufferPtr->vertex_2.data = vertex_data;


		// Set bottom right vertex data
		m_BufferPtr->vertex_3.position = vertex_3_position;
		m_BufferPtr->vertex_3.texture[0] = index_1 + 2;
		m_BufferPtr->vertex_3.texture[1] = index_2 + 2;
		m_BufferPtr->vertex_3.texture[2] = index_3 + 2;
		m_BufferPtr->vertex_3.texture[3] = index_4 + 2;
		m_BufferPtr->vertex_3.data = vertex_data;


		// Set top right vertex data
		m_BufferPtr->vertex_4.position = vertex_4_position;
		m_BufferPtr->vertex_4.texture[0] = index_1 + 3;
		m_BufferPtr->vertex_4.texture[1] = index_2 + 3;
		m_BufferPtr->vertex_4.texture[2] = index_3 + 3;
		m_BufferPtr->vertex_4.texture[3] = index_4 + 3;
		m_BufferPtr->vertex_4.data = vertex_data;

		m_BufferPtr++;
		m_CurrentBatchSize++;
		m_BatchEndPosition++;
	}
	
	//-------------------------------------------------------------------------------------

	bool SpriteRenderer::is_sprite_visible(const BoundingBox& sprite)
	{
		return true;
	}

	//-------------------------------------------------------------------------------------

	SpriteRenderer::SpriteRenderer(const Version& opengl_version, const SpriteRendererSettings& settings, const SpriteRendererProperties& properties)
		: m_SpritesPerStreamBuffer(properties.sprites_per_batch * properties.buffer_allocation_bias),
		m_TexturePack(opengl_version),
		m_Properties(properties),
		m_BatchStartPosition(0),
		m_BatchEndPosition(0),
		m_CurrentBatchSize(0),
		m_Settings(settings),
		m_BatchStarted(false),
		m_BatchEnded(true)
	{
		initialize_renderer(opengl_version);
	}
	
	//-------------------------------------------------------------------------------------

	void SpriteRenderer::begin_batch(const Camera & camera)
	{
		CBN_Assert(!m_BatchStarted && m_BatchEnded, "Cannot start a new batch while batching is currently active");

		// Reset batch statistics
		m_BatchEnded = false;
		m_BatchStarted = true;
		m_CurrentBatchSize = 0;
		m_BatchStartPosition = m_BatchEndPosition;

		// Pre-calculate required camera data
		m_ViewProjectionMatrix = camera.to_view_projection_matrix();
		//TODO: m_CameraBoundingBox = camera.get_view_bounding_box();


		// If there is no space at the end of the buffer for another batch, 
		// then we should wrap back around to the start of the buffer
		if(m_BatchStartPosition + m_Properties.sprites_per_batch >= m_SpritesPerStreamBuffer)
		{
			m_StreamBuffer->reallocate();
			m_BatchStartPosition = 0;
			m_BatchEndPosition = 0;
		}

		// Map the buffer
		m_BufferPtr = reinterpret_cast<SpriteLayout*>(m_StreamBuffer->map(m_BatchStartPosition * sizeof(SpriteLayout), m_Properties.sprites_per_batch * sizeof(SpriteLayout)));
	}
	
	//-------------------------------------------------------------------------------------

	void SpriteRenderer::submit(const BoundingBox& sprite)
	{
		if(m_Settings.cull_outside_camera && !is_sprite_visible(sprite))
			return;
	
		push_sprite_to_buffer(sprite, 0, 0, 0, 0, c_EmptyVertexData);
	}

	//-------------------------------------------------------------------------------------

	void SpriteRenderer::submit(const BoundingBox& sprite, const glm::uvec4& vertex_data)
	{
		if(m_Settings.cull_outside_camera && !is_sprite_visible(sprite))
			return;

		push_sprite_to_buffer(sprite, 0, 0, 0, 0, vertex_data);
	}
	
	//-------------------------------------------------------------------------------------

	void SpriteRenderer::submit(const BoundingBox& sprite, const Name& texture_name_1, const glm::uvec4& vertex_data)
	{
		if(m_Settings.cull_outside_camera && !is_sprite_visible(sprite))
			return;

		push_sprite_to_buffer(sprite, m_TexturePack.position_of(texture_name_1), 0, 0, 0, vertex_data);
	}
	
	//-------------------------------------------------------------------------------------

	void SpriteRenderer::submit(const BoundingBox& sprite, const Name& texture_name_1, const Name& texture_name_2, const glm::uvec4& vertex_data)
	{
		if(m_Settings.cull_outside_camera && !is_sprite_visible(sprite))
			return;

		push_sprite_to_buffer(sprite, m_TexturePack.position_of(texture_name_1), m_TexturePack.position_of(texture_name_2), 0, 0, vertex_data);
	}
	
	//-------------------------------------------------------------------------------------

	void SpriteRenderer::submit(const BoundingBox& sprite, const Name& texture_name_1, const Name& texture_name_2, const Name& texture_name_3, const glm::uvec4& vertex_data)
	{
		if(m_Settings.cull_outside_camera && !is_sprite_visible(sprite))
			return;

		push_sprite_to_buffer(sprite, m_TexturePack.position_of(texture_name_1), m_TexturePack.position_of(texture_name_2), m_TexturePack.position_of(texture_name_3), 0, vertex_data);
	}
	
	//-------------------------------------------------------------------------------------

	void SpriteRenderer::submit(const BoundingBox& sprite, const Name& texture_name_1, const Name& texture_name_2, const Name& texture_name_3, const Name& texture_name_4, const glm::uvec4& vertex_data)
	{
		if(m_Settings.cull_outside_camera && !is_sprite_visible(sprite))
			return;

		push_sprite_to_buffer(sprite, m_TexturePack.position_of(texture_name_1), m_TexturePack.position_of(texture_name_2), m_TexturePack.position_of(texture_name_3), m_TexturePack.position_of(texture_name_4), vertex_data);
	}

	//-------------------------------------------------------------------------------------

	void SpriteRenderer::end_batch()
	{
		CBN_Assert(m_BatchStarted, "Cannot end an unstarted batch");

		m_BatchStarted = false;
		m_BatchEnded = true;

		// Finalise changes to the stream buffer by unmapping it
		m_StreamBuffer->unmap();
	}

	//-------------------------------------------------------------------------------------

	void SpriteRenderer::render(const SRes<ShaderProgram>& shader)
	{
		CBN_Assert(m_BatchEnded, "Cannot render an unfinished batch");

		// Bind the vertex array, texture pack and shader
		m_TexturePack.bind();
		m_VertexArray.bind();
		shader->bind();

		glDrawElements(GL_TRIANGLES, m_CurrentBatchSize * c_IndicesPerSprite, GL_UNSIGNED_INT, (void*)(m_BatchStartPosition * c_IndicesPerSprite * sizeof(uint32_t)));

	}

	//-------------------------------------------------------------------------------------

	bool SpriteRenderer::is_batch_started() const
	{
		return m_BatchStarted;
	}

	//-------------------------------------------------------------------------------------

	bool SpriteRenderer::is_batch_full() const
	{
		return m_CurrentBatchSize == m_Properties.sprites_per_batch;
	}

	//-------------------------------------------------------------------------------------

	int SpriteRenderer::batch_size() const
	{
		return m_CurrentBatchSize;
	}

	//-------------------------------------------------------------------------------------

	SpriteRendererSettings SpriteRenderer::settings() const
	{
		return m_Settings;
	}

	//-------------------------------------------------------------------------------------

	SpriteRendererProperties SpriteRenderer::properties() const
	{
		return m_Properties;
	}

	//-------------------------------------------------------------------------------------

	void SpriteRenderer::configure(const SpriteRendererSettings& settings)
	{
		m_Settings = settings;
	}

	//-------------------------------------------------------------------------------------

	void SpriteRenderer::set_texture_pack(const TexturePack& textures)
	{
		m_TexturePack = textures;
	}

	//-------------------------------------------------------------------------------------
}
