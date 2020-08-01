#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Camera.hpp"
#include "Resources/ShaderProgram.hpp"
#include "OpenGL/OpenGL.hpp"
#include "OpenGL/GLObjects.hpp"
#include "OpenGL/VertexDataDescriptor.hpp"

namespace cbn
{

	template<typename VertexData>
	class QuadRenderer
	{
	public:


		struct QuadData
		{
			VertexData ul_vertex;
			VertexData ll_vertex;
			VertexData lr_vertex;
			VertexData ur_vertex;
		};

	private:

#pragma pack(push, 1)
		
		struct VertexLayout
		{
			glm::vec2 position;
			VertexData vertex_data;
		};

		struct QuadLayout
		{
			VertexLayout ul_vertex;
			VertexLayout ll_vertex;
			VertexLayout lr_vertex;
			VertexLayout ur_vertex;
		};

#pragma pack(pop)

		const GLsizeiptr m_BufferSectionByteSize;
		const GLsizeiptr m_TotalBufferByteSize;
		const uint64_t m_MaximumBatchSize;
		const uint64_t m_BufferSections;

		VertexArrayObject m_VertexArrayObject;
		VertexBuffer m_StreamBuffer;
		IndexBuffer m_IndexBuffer;

		std::vector<GLsync> m_BufferSectionLocks;
		QuadLayout* m_SectionWritePointer;
		QuadLayout* m_BufferBasePointer;
		uint64_t m_SectionIndex;
		
		glm::mat4 m_ViewProjectionMatrix;
		GLsizei m_CurrentBatchSize;
		bool m_BatchStarted;

		static bool should_cull_quad(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4);

		void create_vertex_array_object(const VertexDataDescriptor& descriptor);

		void create_stream_buffer();

		void create_index_buffer();

		void lock_section(const uint32_t section_index);

		void unlock_section(const uint32_t section_index);

	public:

		QuadRenderer(const VertexDataDescriptor& descriptor, const unsigned batch_size = 16383, const unsigned buffers = 3);

		void begin(Camera& camera);

		void begin(const glm::mat4& view_matrix, const glm::mat4& projection_matrix);

		void submit(const glm::vec2& base_size, const glm::mat4& transform_matrix, const VertexData& quad_data);

		bool try_submit(const glm::vec2& base_size, const glm::mat4& transform_matrix, const VertexData& quad_data);

		void submit(const glm::vec2& base_size, const glm::mat4& transform_matrix, const QuadData& quad_data);

		bool try_submit(const glm::vec2& base_size, const glm::mat4& transform_matrix, const QuadData& quad_data);
	
		void end();

		void render(ShaderProgram& shader_program);

		int get_batch_capacity() const;

		int get_batch_size() const;

		bool is_batch_full() const;

	};

}


#include "QuadRenderer.tpp"