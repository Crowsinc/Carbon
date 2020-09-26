#pragma once

/*
		Rendering:

		Goal: 100k sprites of different textures at 100fps.


		TODO: Update AABB to take in a rect and be performed with a pre-transformed sprite. 
		That way we actually skip all the CPU costs of performing the transforms. 



		///

		- GL33 round robin buffers (or single buffer split)
		- GLmap unsynchronised, with my own fast synchronisation. 

	*/

#include <stdint.h>

#include "OpenGL/VertexArrayObject.hpp"
#include "Resources/ShaderProgram.hpp"
#include "Resources/StaticBuffer.hpp"
#include "Resources/StreamBuffer.hpp"
#include "../Utility/Version.hpp"
#include "TexturePack.hpp"
#include "Sprite.hpp"
#include "Camera.hpp"

namespace cbn
{

	class SpriteRenderer
	{
	public:

		struct Properties
		{
			Version opengl_version = Version{3,3};
			uint32_t batch_size = 16384;
			bool cull_sprites = true;
		};

	private:

#pragma pack(push, 1)

		struct VertexLayout
		{
			glm::vec2 position;
			glm::u16vec4 uvs;
			glm::uvec2 data; // first int: RGBA, second int: samplers, 3rd/4th int: user data. 
		}; // 32 bytes

		struct QuadLayout
		{
			VertexLayout ul_vertex;
			VertexLayout ll_vertex;
			VertexLayout lr_vertex;
			VertexLayout ur_vertex;
		}; // 128 bytes

#pragma pack(pop)

		//TODO: drop vertex layout to only have 2 textures, and try to hit 32 bytes per vertex
		//TODO: make it so we can render the same geometry twice by calling render twice with two shaders

		struct BatchObjectData
		{
			SRes<StreamBuffer> stream_buffer;
			SRes<StaticBuffer> index_buffer;
			VertexArrayObject vertex_array;
			GLsync stream_buffer_fence;
		};

		static SRes<ShaderProgram> s_DefaultShaderProgram;

		std::vector<BatchObjectData> m_BatchObjects;
		const Properties m_Properties;
		TexturePack m_TextureSet;
		QuadLayout* m_BufferPtr;
		glm::mat4 m_VPMatrix;
		uint32_t m_BatchSize;
		bool m_BatchStarted;
		bool m_BatchEnded;

		static void create_default_shader();

		//TODO: use circle approximation culling in world space (not [-1,1])
		bool should_cull(const Sprite& sprite);
		
	public:

		SpriteRenderer(const Properties properties);

		void begin(const Camera& camera);

		void submit(const Sprite& sprite);

		void end();

		void render();

		void render(const SRes<ShaderProgram>& shader);

		bool is_batch_started() const;
		
		bool is_batch_full() const;

		Properties get_properties() const;

		void set_textures(const TexturePack& textures);
	};

}