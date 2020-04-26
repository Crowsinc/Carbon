#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "OpenGL.hpp"
#include "Camera.hpp"
#include "ShaderProgram.hpp"

namespace cbn
{

	template<typename VertexData>
	class QuadRenderer
	{
	public:

		struct LayoutAttribute
		{
			GLboolean normalize;
			GLenum type;
			GLint count;
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

		std::vector<GLsync> m_BufferSectionLocks;
		VertexArrayObject m_VertexArrayObject;
		QuadLayout* m_SectionWritePointer;
		glm::mat4 m_ViewProjectionMatrix;
		QuadLayout* m_BufferBasePointer;
		VertexBuffer m_StreamBuffer;
		GLsizei m_CurrentBatchSize;
		IndexBuffer m_IndexBuffer;
		uint64_t m_SectionIndex;
		bool m_BatchStarted;

		static bool should_cull_quad(const glm::vec2& ul_vertex_position, const glm::vec2& ll_vertex_position, const glm::vec2& lr_vertex_position, const glm::vec2& ur_vertex_position);

		static int find_byte_size(const LayoutAttribute& attribute);

		void create_stream_buffer();

		void create_index_buffer();

		void create_vertex_array_object(const std::vector<LayoutAttribute>& vertex_attributes);

		void lock_section(const uint32_t section_index);

		void unlock_section(const uint32_t section_index);

	public:

		QuadRenderer(const std::vector<LayoutAttribute>& vertex_attributes);

		void begin(Camera& camera);

		void begin(const glm::mat4& view_matrix, const glm::mat4& projection_matrix);

		//TODO: use Transform class?

		void submit(const glm::vec2& base_size, const glm::mat4& transform_matrix, const VertexData& quad_data);

		bool try_submit(const glm::vec2& base_size, const glm::mat4& transform_matrix, const VertexData& quad_data);

		void submit(const glm::vec2& base_size, const glm::mat4& transform_matrix, const VertexData& left_half_data, const VertexData& right_half_data);

		bool try_submit(const glm::vec2& base_size, const glm::mat4& transform_matrix, const VertexData& left_half_data, const VertexData& right_half_data);

		void submit(const glm::vec2& base_size, const glm::mat4& transform_matrix, const VertexData& ul_vertex_data, const VertexData& ll_vertex_data, const VertexData& lr_vertex_data, const VertexData& ur_vertex_data);

		bool try_submit(const glm::vec2& base_size, const glm::mat4& transform_matrix, const VertexData& ul_vertex_data, const VertexData& ll_vertex_data, const VertexData& lr_vertex_data, const VertexData& ur_vertex_data);

		void end();

		void render(ShaderProgram& shader_program);

		int get_batch_capacity() const;

		int get_batch_size() const;

		bool is_batch_full() const;

	};

}


#include "QuadRenderer.tpp"