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

	/*
			Sprite:

				- Should have the most basic functionality, but with the ability to build it up easily

				- Should be able to be extended, or used through composition

				- Should have helper functions

				-

				Vertex Layout : 32 bytes
				{
					position data {glm::vec2} : 8 bytes
					custom_data_1 {uint32_t}  : 4 bytes
					custom_data_2 {uint32_t}  : 4 bytes
					custom_data_3 {uint32_t}  : 4 bytes
					custom_data_4 {uint32_t}  : 4 bytes
					custom_data_5 {uint32_t}  : 4 bytes
					custom_data_6 {uint32_t}  : 4 bytes
				}

				Each custom_data can be either:
					1. texture data for one texture
					2. custom 32bit unsigned integer
					3. RGBA colour


				Set custom data through...

				class TextureData

				Class SpriteData


		*/


#include <stdint.h>
#include <variant>

#include "OpenGL/VertexArrayObject.hpp"
#include "Resources/ShaderProgram.hpp"
#include "Resources/StaticBuffer.hpp"
#include "Resources/StreamBuffer.hpp"
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
		Version opengl_version = Version{3,3};
		uint32_t maximum_batch_size = 16384;
		bool cull_outside_camera = true;
	};

	using SpriteData = std::variant<uint32_t, Name, Colour>;

	class SpriteRenderer
	{
	private:

		struct BatchObjects
		{
			SRes<StreamBuffer> stream_buffer;
			VertexArrayObject vertex_array;
			GLsync fence;
		};

#pragma pack(push, 1)
		
		struct VertexLayout
		{
			glm::vec2 position;
			uint32_t data_1;
			uint32_t data_2;
			uint32_t data_3;
			uint32_t data_4;
			uint32_t data_5;
			uint32_t data_6;
		};

		struct QuadLayout
		{
			VertexLayout top_left_vertex;
			VertexLayout bot_left_vertex;
			VertexLayout bot_right_vertex;
			VertexLayout top_right_vertex;
		};

#pragma pack(pop)

		const SpriteRendererProperties m_Properties;
		std::vector<BatchObjects> m_BatchObjects;
		bool m_BatchStarted, m_BatchEnded;
		SRes<StaticBuffer> m_IndexBuffer;
		glm::mat4 m_ViewProjectionMatrix;
		BoundingBox m_CameraBoundingBox;
		TexturePack m_TexturePack;
		int m_CurrentBatchIndex;
		QuadLayout* m_BatchData;
		int m_BatchSize;

		uint32_t pack_texture(const Name& texture_name);

		uint32_t extract_data(const SpriteData& data);

		bool should_cull(const BoundingBox& sprite);

		SRes<StaticBuffer> create_index_buffer();

		void create_batch_objects();

	public:

		SpriteRenderer(const SpriteRendererProperties& properties);

		void begin_batch(const Camera& camera);

		void submit(BoundingBox sprite, const SpriteData sprite_data_0, const SpriteData sprite_data_1 = 0, const SpriteData sprite_data_2 = 0, const SpriteData sprite_data_3 = 0, const SpriteData sprite_data_4 = 0, const SpriteData sprite_data_5 = 0);

		void end_batch();

		void render(const ShaderProgram& shader);

		bool is_batch_started() const;

		bool is_batch_full() const;

		int batch_size() const;

		SpriteRendererProperties get_settings() const;

		void set_texture_pack(const TexturePack& textures);
	};

}
