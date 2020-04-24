#include "QuadRenderer.hpp"

#include "../Diagnostics/Assert.hpp"
#include "QuadRenderer.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	bool QuadRenderer<VertexData>::should_cull_quad(const glm::vec2& ul_vertex_position, const glm::vec2& ll_vertex_position, const glm::vec2& lr_vertex_position, const glm::vec2& ur_vertex_position) 
	{
		//TODO: optimise

		// Cull the quad if the rightmost point is past the left of the camera
		if(std::max(std::max(ul_vertex_position.x, ur_vertex_position.x), std::max(ll_vertex_position.x, lr_vertex_position.x)) < -1.0f)
		{
			return true;
		}

		// Cull the quad if the leftmost point is past the right of the camera
		if(std::min(std::min(ul_vertex_position.x, ur_vertex_position.x), std::min(ll_vertex_position.x, lr_vertex_position.x)) > 1.0f)
		{
			return true;
		}

		// Cull the quad if the topmost point is past the bottom of the camera
		if(std::max(std::max(ul_vertex_position.y, ll_vertex_position.y), std::max(ur_vertex_position.y, lr_vertex_position.y)) < -1.0f)
		{
			return true;
		}

		// Cull the quad if the bottommost point is past the top of the camera
		if(std::min(std::min(ul_vertex_position.y, ll_vertex_position.y), std::min(ur_vertex_position.y, lr_vertex_position.y)) > 1.0f)
		{
			return true;
		}

		return false;
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	int QuadRenderer<VertexData>::find_byte_size(const LayoutAttribute& attribute)
	{
		// Get the byte size of the type
		int type_byte_size = 0;
		switch(attribute.type)
		{
			case GL_BYTE:
			case GL_UNSIGNED_BYTE:
			{
				type_byte_size = sizeof(uint8_t);
				break;
			}
			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
			{
				type_byte_size = sizeof(uint16_t);
				break;
			}
			case GL_INT:
			case GL_UNSIGNED_INT:
			case GL_INT_2_10_10_10_REV:
			case GL_UNSIGNED_INT_2_10_10_10_REV:
			case GL_UNSIGNED_INT_10F_11F_11F_REV:
			{
				type_byte_size = sizeof(uint32_t);
				break;
			}
			case GL_FIXED:
			case GL_FLOAT:
			{
				type_byte_size = sizeof(float);
				break;
			}
			case GL_HALF_FLOAT:
			{
				type_byte_size = sizeof(float) / 2;
				break;
			}
			case GL_DOUBLE:
			{
				type_byte_size = sizeof(double);
				break;
			}
		}

		// The total byte size will be the type byte size multiplied by the amount of that type
		return attribute.count * type_byte_size;
	}

	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline void QuadRenderer<VertexData>::create_stream_buffer()
	{
		// Bind the stream buffer 
		m_StreamBuffer.bind();

		// Allocate immutable storage for the buffer with the write and persisten bits
		// This will allow us to map the buffer only once and keep dynamically updating it
		const GLbitfield buffer_flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
		glBufferStorage(GL_ARRAY_BUFFER, m_TotalBufferByteSize, NULL, buffer_flags);

		// Map the entire stream buffer using persistent mapping
		const GLbitfield mapping_flags = buffer_flags | GL_MAP_FLUSH_EXPLICIT_BIT;
		m_BufferBasePointer = reinterpret_cast<QuadLayout*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, m_TotalBufferByteSize, mapping_flags));

		// Make sure the buffer was successfully mapped, if it wasn't then an error 
		// occurred with the QuadRenderer code, so we should assert out to fix it
		CBN_Assert(m_BufferBasePointer != NULL, "Unable to map stream buffer");

		// While we have mapped the entire buffer at once, we will only work with a 
		// specific range of the buffer at a time. This will emulate multi-buffering
		// and will allow us to modify one section of the buffer while the GPU is
		// rendering the previous section. For this reason, we need to setup a separate
		// fence for each section of the buffer in order to properly synchronize the buffer
		for(auto i = 0; i < m_BufferSections; i++)
		{
			// For each section, add an initial GPU fence
			m_BufferSectionLocks.push_back(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
		}
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline void QuadRenderer<VertexData>::create_index_buffer()
	{
		// Bind the index buffer
		m_IndexBuffer.bind();

		// Start generating all the indices for the stream buffer, each quad will require 6 indices
		// and will be specified with an unsigned integer. Note that the buffer is 'm_BufferSections'
		// times bigger than the maximum batch size, so the total number of indices will be:
		// index_count = buffer sections * quads per section * 6 indices per quad
		const uint32_t index_count = m_BufferSections * m_MaximumBatchSize * 6;

		// Create a vector to store the indices and pre-allocate it for the required indices
		std::vector<uint32_t> buffer_indices;
		buffer_indices.reserve(index_count);

		// Generate all the requires indices for the stream buffer
		for(uint32_t index = 0, base_vertex = 0; index < index_count; index += 6, base_vertex += 4)
		{
			buffer_indices.push_back(base_vertex + 0);
			buffer_indices.push_back(base_vertex + 1);
			buffer_indices.push_back(base_vertex + 3);
			buffer_indices.push_back(base_vertex + 3);
			buffer_indices.push_back(base_vertex + 1);
			buffer_indices.push_back(base_vertex + 2);
		}

		// Allocate immutable storage for the index buffer and fill it with our buffer indices
		glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, buffer_indices.size() * sizeof(uint32_t), buffer_indices.data(), NULL);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline void QuadRenderer<VertexData>::create_vertex_array_object(const std::vector<LayoutAttribute>& vertex_attributes)
	{
		// Start by binding the vertex array object
		// Any buffers created after the binding will 
		// be associated with this vertex array object
		m_VertexArrayObject.bind();

		// Create the index buffer
		create_index_buffer();

		// Create the stream buffer
		create_stream_buffer();

		// Setup the attribute pointers for the vertex layout, starting with
		// the position, then followed by the user defined vertex attributes
		glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(VertexLayout), reinterpret_cast<GLvoid*>(0));
		for(int offset = 8, i = 0; i < vertex_attributes.size(); i++) // TODO: make sure no more than 15 attributes exist in total
		{
			const auto attribute = vertex_attributes[i];
			glVertexAttribPointer(i + 1, attribute.count, attribute.type, attribute.normalize, sizeof(VertexLayout), reinterpret_cast<GLvoid*>(offset));

			// Increase the offset by the last attribute
			offset += find_byte_size(attribute);
		}
		
		// Enable the vertex attributes
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Unbind the vertex array object to finish the setup process
		m_VertexArrayObject.unbind();
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline void QuadRenderer<VertexData>::lock_section(const uint32_t section_index)
	{
		// Start by deleting the any previously set locks for this section
		glDeleteSync(m_BufferSectionLocks[section_index]);

		// To lock the buffer section, update the GPU fence for the section
		// This will place a fence in the GPU command queue, hence the buffer will
		// not be able to be unlocked until all GPU commands before this fence are completed
		m_BufferSectionLocks[section_index] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline void QuadRenderer<VertexData>::unlock_section(const uint32_t section_index)
	{
		// To unlock a section, we will capture the thread here until the 
		// last GPU fence for that buffer section has been signaled
		const GLsync section_fence = m_BufferSectionLocks[section_index];

		// Check if the GPU fence for the section has already been signalled.
		// If it has, then we want to return immediately without waiting at all
		GLenum fence_status = glClientWaitSync(section_fence, NULL, NULL);
		if(fence_status == GL_ALREADY_SIGNALED || fence_status == GL_CONDITION_SATISFIED)
		{
			return;
		}

		// If we are here, then the fence has not been signalled yet. We will  
		// capture the thread until the fence is signalled using a 1 second timout
		do
		{
			// Using a 1 second timeout, wait for gpu commands to flush
			fence_status = glClientWaitSync(section_fence, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
		} while(fence_status != GL_ALREADY_SIGNALED && fence_status != GL_CONDITION_SATISFIED);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline QuadRenderer<VertexData>::QuadRenderer(const std::vector<LayoutAttribute>& vertex_attributes)
		: m_TotalBufferByteSize(16383 * sizeof(QuadLayout) * 3),
		m_BufferSectionByteSize(16383 * sizeof(QuadLayout)),
		m_MaximumBatchSize(16383),
		m_BufferSections(3),
		m_SectionWritePointer(nullptr),
		m_BufferBasePointer(nullptr),
		m_CurrentBatchSize(0),
		m_BatchStarted(false),
		m_SectionIndex(0)
	{
		// Setup the vertex array object for the quad renderer
		create_vertex_array_object(vertex_attributes);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline void QuadRenderer<VertexData>::begin(Camera& camera)
	{
		begin(camera.to_view_matrix(), camera.to_projection_matrix());
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline void QuadRenderer<VertexData>::begin(const glm::mat4& view_matrix, const glm::mat4& projection_matrix)
	{
		// Ensure that the batch has not already been started
		CBN_Assert(!m_BatchStarted, "Cannot begin a batch while one is already in progress");

		// Reset batching data
		m_BatchStarted = true;
		m_CurrentBatchSize = 0;

		// Precalculate the view-projection matrix for this batching run
		m_ViewProjectionMatrix = vp_fast_multiply_2d(view_matrix, projection_matrix);

		// Increase the buffer section index 
		m_SectionIndex = (m_SectionIndex + 1) % m_BufferSections;

		// Update the write pointer to the base of the new section. Since the write pointer
		// is in terms of the QuadLayout, the offset will be a multiple of the maximum batch size
		// The pointer is decremented by one because the first submission will increment it before it is used
		m_SectionWritePointer = m_BufferBasePointer + (m_SectionIndex * m_MaximumBatchSize) - 1;

		// Unlock the new section we are using, waiting for the GPU to stop using it if we have to
		unlock_section(m_SectionIndex);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline void QuadRenderer<VertexData>::submit(const glm::vec2& base_size, const glm::mat4& transform, const VertexData& quad_data)
	{
		submit(base_size, transform, quad_data, quad_data, quad_data, quad_data);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline bool QuadRenderer<VertexData>::try_submit(const glm::vec2& base_size, const glm::mat4& transform, const VertexData& quad_data)
	{
		return try_submit(base_size, transform, quad_data, quad_data, quad_data, quad_data);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline void QuadRenderer<VertexData>::submit(const glm::vec2& base_size, const glm::mat4& transform, const VertexData& left_half_data, const VertexData& right_half_data)
	{
		submit(base_size, transform, left_half_data, left_half_data, right_half_data, right_half_data);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline bool QuadRenderer<VertexData>::try_submit(const glm::vec2& base_size, const glm::mat4& transform, const VertexData& left_half_data, const VertexData& right_half_data)
	{
		return try_submit(base_size, transform, left_half_data, left_half_data, right_half_data, right_half_data);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline void QuadRenderer<VertexData>::submit(const glm::vec2& base_size, const glm::mat4& transform, const VertexData& ul_vertex_data, const VertexData& ll_vertex_data, const VertexData& lr_vertex_data, const VertexData& ur_vertex_data)
	{
		// Make sure we don't exceed the maximum batch size
		CBN_Assert(m_CurrentBatchSize < m_MaximumBatchSize, "Cannot exceed maximum batch size");

		// Use the view projection and transform matrices to get the full mvp matrix
		const glm::mat4 mvp_matrix = mvp_fast_multiply_2d(m_ViewProjectionMatrix, transform);
		const glm::vec2 base_half_size = base_size * 0.5f;

		// Use the matrix and the base size to determine all the vertex positions for the quad
		const glm::vec2 ul_vertex_position = fast_2d_transform({-base_half_size.x, base_half_size.y}, mvp_matrix);
		const glm::vec2 ll_vertex_position = fast_2d_transform(-base_half_size, mvp_matrix);
		const glm::vec2 lr_vertex_position = fast_2d_transform({base_half_size.x, -base_half_size.y}, mvp_matrix);
		const glm::vec2 ur_vertex_position = fast_2d_transform(base_half_size, mvp_matrix);

		// If the quad is outside of the camera's view, return from the function and dont try to render it
		if(should_cull_quad(ul_vertex_position, ll_vertex_position, lr_vertex_position, ur_vertex_position))
		{
			return;
		}

		// If we are here, the quad did not get culled so increase the batch count
		// and then start packing its vertex data into the current mapped buffer
		m_CurrentBatchSize++;

		// Increase the section write pointer so that we write the quad data into
		// the next location in the buffer section. Note that the begin() function
		// will set the pointer 1 location before the first location of the section
		// so that the first quad does not skip the first location of the section. 
		m_SectionWritePointer++;

		// Start inserting the quad vertex data into the stream buffer
		// Note that the vertices must be ordered in counter clockwise
		// order starting at the upper left vertex. 

		// Set upper left vertex data
		m_SectionWritePointer->ul_vertex.position = ul_vertex_position;
		m_SectionWritePointer->ul_vertex.vertex_data = ul_vertex_data;

		// Set lower left vertex data
		m_SectionWritePointer->ll_vertex.position = ll_vertex_position;
		m_SectionWritePointer->ll_vertex.vertex_data = ll_vertex_data;

		// Set lower right vertex data
		m_SectionWritePointer->lr_vertex.position = lr_vertex_position;
		m_SectionWritePointer->lr_vertex.vertex_data = lr_vertex_data;

		// Set upper right vertex data
		m_SectionWritePointer->ur_vertex.position = ur_vertex_position;
		m_SectionWritePointer->ur_vertex.vertex_data = ur_vertex_data;
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline bool QuadRenderer<VertexData>::try_submit(const glm::vec2& base_size, const glm::mat4& transform, const VertexData& ul_vertex_data, const VertexData& ll_vertex_data, const VertexData& lr_vertex_data, const VertexData& ur_vertex_data)
	{
		if(m_CurrentBatchSize < m_MaximumBatchSize)
		{
			submit(base_size, transform, ul_vertex_data, ll_vertex_data, lr_vertex_data, ur_vertex_data);
		}
		return false;
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline void QuadRenderer<VertexData>::end()
	{
		// Ensure that the batch has already been started
		CBN_Assert(m_BatchStarted, "Cannot end a batch without first beginning one");

		// Update the batch flag to reflect that the batch is finished
		m_BatchStarted = false;

		// Since we have finished modifying the the current buffer section, flush the changed to the GPU
		glFlushMappedBufferRange(GL_ARRAY_BUFFER, m_SectionIndex * m_BufferSectionByteSize, m_CurrentBatchSize * sizeof(QuadLayout));
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline void QuadRenderer<VertexData>::render(ShaderProgram& shader_program)
	{
		// Before we can render the batch, we need to make 
        // sure that the shader and vertex array are bound
		m_VertexArrayObject.bind();
		shader_program.bind();

		// Perform the draw call using an index buffer. Since there are 6 indices per quad we draw, the draw count
		// will be the current batch size multiplied by 6 indices. We also need to offset the index buffer by the current section
		glDrawElements(GL_TRIANGLES, m_CurrentBatchSize * 6, GL_UNSIGNED_INT, reinterpret_cast<void*>(m_SectionIndex * m_MaximumBatchSize * 6 * sizeof(uint32_t)));

		// Lock the current section so that we don't override the 
		// section data until the draw call has been completed by the GPU
		lock_section(m_SectionIndex);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline int QuadRenderer<VertexData>::get_batch_capacity() const
	{
		return m_MaximumBatchSize;
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline int QuadRenderer<VertexData>::get_batch_size() const
	{
		return m_CurrentBatchSize;
	}
	
	//-------------------------------------------------------------------------------------

	template<typename VertexData>
	inline bool QuadRenderer<VertexData>::is_batch_full() const
	{
		return m_CurrentBatchSize == m_MaximumBatchSize;
	}
	
	//-------------------------------------------------------------------------------------

}