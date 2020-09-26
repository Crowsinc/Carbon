#include "SpriteRenderer.hpp"

namespace cbn
{

	const char* vertex_source = "#version 400 core\n"
		"layout(location = 0) in vec4 VertexData;\n"
		"layout(location = 1) in vec2 uvs;\n"
		"out vec2 _uvs;\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = vec4(VertexData.xy, 0.0, 1.0);\n"
		"	_uvs = uvs;\n"
		"}\n"
		";";

	const char* fragment_source = "#version 400 core\n"
		"in vec2 _uvs;\n"
		"out vec4 fragColour;\n"
		"uniform sampler2D text[16];"
		"void main(void)\n"
		"{\n"
		"	fragColour = texture(text[0], _uvs);\n"
		"}\n"
		";";

	//-------------------------------------------------------------------------------------

	SRes<ShaderProgram> SpriteRenderer::s_DefaultShaderProgram;

	//-------------------------------------------------------------------------------------

	void SpriteRenderer::create_default_shader()
	{
		std::string error_log;
		SRes<Shader> vertex_shader = Shader::Compile(vertex_source, Shader::Stage::VERTEX, error_log);
		CBN_Assert(vertex_shader != nullptr, "Could not compile default vertex shader | " + error_log);

		SRes<Shader> fragment_shader = Shader::Compile(fragment_source, Shader::Stage::FRAGMENT, error_log);
		CBN_Assert(fragment_shader != nullptr, "Could not compile default fragment shader | " + error_log);

		s_DefaultShaderProgram = ShaderProgram::Create(vertex_shader, nullptr, fragment_shader, error_log);
		CBN_Assert(fragment_shader != s_DefaultShaderProgram, "Could not link default shader program | " + error_log);
	}
	
	//-------------------------------------------------------------------------------------

	SpriteRenderer::SpriteRenderer(const Properties properties)
		: m_Properties(properties)
	{
		// If the default shader program has not been made yet, make it
		if(!s_DefaultShaderProgram)
			create_default_shader();


	}
	
	//-------------------------------------------------------------------------------------

	void SpriteRenderer::begin(const Camera& camera)
	{
		CBN_Assert(!m_BatchStarted, "Cannot start batch when one is already started");

		// Reset batch statistics
		m_BatchSize = 0;
		m_BatchEnded = false;
		m_BatchStarted = true;

		// Precalculate view-projection matrix for the whole batch
		m_VPMatrix = build_vp_matrix(camera.to_view_matrix(), camera.to_projection_matrix());

		// Switch to next buffer
		m_BufferIndex = (m_BufferIndex + 1) % m_Buffers.size();
		auto& stream_buffer = m_Buffers[m_BufferIndex];

		// Unlock the new buffer we are using, waiting for the GPU to stop using it if we have to
		unlock_buffer(stream_buffer);

		// Map the buffer we will use for this batch
		m_BufferPtr = static_cast<QuadLayout*>(stream_buffer->map());
	}
	
	//-------------------------------------------------------------------------------------

	void SpriteRenderer::submit(const Sprite& sprite)
	{
		CBN_Assert(m_BatchStarted, "Cannot submit sprite without starting new batch");
		CBN_Assert(!is_batch_full(), "Cannot submit to full batch");

		// If the sprite is out of view and the cull option is true, dont submit
		if(m_Properties.cull_sprites && should_cull(sprite))
			return;
		
		// Build the full mvp matrix for the sprite and get all its vertex positions
		const glm::mat4 mvp_matrix = build_mvp_matrix(m_VPMatrix, sprite.to_transform_matrix());
		
		const Sprite::Vertices local_vertices = sprite.get_local_vertices();
		const glm::vec2 ul_vertex_position = transform(local_vertices.ul_vertex, mvp_matrix);
		const glm::vec2 ll_vertex_position = transform(local_vertices.ll_vertex, mvp_matrix);
		const glm::vec2 lr_vertex_position = transform(local_vertices.lr_vertex, mvp_matrix);
		const glm::vec2 ur_vertex_position = transform(local_vertices.ur_vertex, mvp_matrix);

		// Set the positions in the buffer
		m_BufferPtr->ul_vertex.position = ul_vertex_position;
		m_BufferPtr->ll_vertex.position = ll_vertex_position;
		m_BufferPtr->lr_vertex.position = lr_vertex_position;
		m_BufferPtr->ur_vertex.position = ur_vertex_position;

		// Set sprite tint in buffer
		const Sprite::Tint tint = sprite.get_tint();
		m_BufferPtr->ul_vertex.quad_tint = tint.ul_vertex;
		m_BufferPtr->ll_vertex.quad_tint = tint.ll_vertex;
		m_BufferPtr->lr_vertex.quad_tint = tint.lr_vertex;
		m_BufferPtr->ur_vertex.quad_tint = tint.ur_vertex;

		// Get sprite texture data
		glm::u8vec4 samplers = {255,255,255,255};
		const Sprite::Textures sprite_textures = sprite.get_textures();
		
		// Main Texture
		const Texture::UVMap main_texture_uvs = m_TextureSet.get_uv_map(sprite_textures.main_texture);
		samplers.x = static_cast<int>(m_TextureSet.get_unit(sprite_textures.main_texture));
		m_BufferPtr->ul_vertex.main_uvs = main_texture_uvs.ul;
		m_BufferPtr->ll_vertex.main_uvs = main_texture_uvs.ll;
		m_BufferPtr->lr_vertex.main_uvs = main_texture_uvs.lr;
		m_BufferPtr->ur_vertex.main_uvs = main_texture_uvs.ur;

		// Aux Texture 1
		if(sprite_textures.aux_texture_1.has_value())
		{
			const Texture::UVMap aux_texture_1_uvs = m_TextureSet.get_uv_map(sprite_textures.aux_texture_1.value());
			samplers.y = static_cast<int>(m_TextureSet.get_unit(sprite_textures.aux_texture_1.value()));

			m_BufferPtr->ul_vertex.aux_uvs_1 = aux_texture_1_uvs.ul;
			m_BufferPtr->ll_vertex.aux_uvs_1 = aux_texture_1_uvs.ll;
			m_BufferPtr->lr_vertex.aux_uvs_1 = aux_texture_1_uvs.lr;
			m_BufferPtr->ur_vertex.aux_uvs_1 = aux_texture_1_uvs.ur;
		}

		// Aux Texture 2
		if(sprite_textures.aux_texture_2.has_value())
		{
			const Texture::UVMap aux_texture_2_uvs = m_TextureSet.get_uv_map(sprite_textures.aux_texture_2.value());
			samplers.y = static_cast<int>(m_TextureSet.get_unit(sprite_textures.aux_texture_2.value()));

			m_BufferPtr->ul_vertex.aux_uvs_2 = aux_texture_2_uvs.ul;
			m_BufferPtr->ll_vertex.aux_uvs_2 = aux_texture_2_uvs.ll;
			m_BufferPtr->lr_vertex.aux_uvs_2 = aux_texture_2_uvs.lr;
			m_BufferPtr->ur_vertex.aux_uvs_2 = aux_texture_2_uvs.ur;
		}

		// Aux Texture 3
		if(sprite_textures.aux_texture_3.has_value())
		{
			const Texture::UVMap aux_texture_3_uvs = m_TextureSet.get_uv_map(sprite_textures.aux_texture_3.value());
			samplers.y = static_cast<int>(m_TextureSet.get_unit(sprite_textures.aux_texture_3.value()));

			m_BufferPtr->ul_vertex.aux_uvs_3 = aux_texture_3_uvs.ul;
			m_BufferPtr->ll_vertex.aux_uvs_3 = aux_texture_3_uvs.ll;
			m_BufferPtr->lr_vertex.aux_uvs_3 = aux_texture_3_uvs.lr;
			m_BufferPtr->ur_vertex.aux_uvs_3 = aux_texture_3_uvs.ur;
		}

		// Set samplers
		m_BufferPtr->ul_vertex.samplers = samplers;
		m_BufferPtr->ll_vertex.samplers = samplers;
		m_BufferPtr->lr_vertex.samplers = samplers;
		m_BufferPtr->ur_vertex.samplers = samplers;

		m_BatchSize++;
		m_BufferPtr++;
	}
	
	//-------------------------------------------------------------------------------------

	void SpriteRenderer::end()
	{
		CBN_Assert(m_BatchStarted, "Cannot end batch without first starting a batch");

		m_BatchEnded = true;
		m_BatchStarted = false;
	
		m_Buffers[m_BufferIndex]->unmap();
	}
	
	//-------------------------------------------------------------------------------------

	void SpriteRenderer::render()
	{
		render(s_DefaultShaderProgram);
	}
	
	//-------------------------------------------------------------------------------------

	void SpriteRenderer::render(const SRes<ShaderProgram>& shader)
	{
		CBN_Assert(m_BatchEnded, "Cannot render batch without ending the batch");
		CBN_Assert(shader != nullptr, "Cannot render with null shader program");

		//shader->bind();
		//m_QuadVAO.bind();
		//m_TextureSet.bind();
		//
		//glDrawElements(GL_TRIANGLES, m_BatchSize * 6, )
		//
		//lock_buffer(m_Buffers[m_BufferIndex]);
	}
	
	//-------------------------------------------------------------------------------------

	bool SpriteRenderer::is_batch_started() const
	{
		return false;
	}
	
	//-------------------------------------------------------------------------------------

	bool SpriteRenderer::is_batch_full() const
	{
		return false;
	}
	
	//-------------------------------------------------------------------------------------

	SpriteRenderer::Properties SpriteRenderer::get_properties() const
	{
		return m_Properties;
	}
	
	//-------------------------------------------------------------------------------------

	void SpriteRenderer::set_textures(const TexturePack& textures)
	{
		CBN_Assert(!m_BatchStarted, "Cannot set textures with an active batch");
		//TODO: check for compatibility (16 textures max, 16384 max size)
	}
	
	//-------------------------------------------------------------------------------------

}