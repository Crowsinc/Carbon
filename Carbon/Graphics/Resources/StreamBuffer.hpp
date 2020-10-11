#pragma once

#include <vector>

#include "../../Utility/Version.hpp"
#include "../../Memory/Resource.hpp"
#include "../OpenGL/Buffer.hpp"
#include "../OpenGL/OpenGL.hpp"

namespace cbn
{

	class StreamBuffer : public Buffer
	{
	public:
		
		static SRes<StreamBuffer> Allocate(const BufferTarget target, const uint64_t byte_size, const bool synchronized, const Version opengl_version);

	private:
		
		const bool m_Synchronized;
		const GLbitfield m_MappingFlags;
		uint64_t m_ByteSize;
		bool m_Mapped;

		StreamBuffer(const BufferTarget target, const uint64_t byte_size, const bool unsynchronized);

	public:

		void* map();
		
		void* map(const uint64_t offset, const uint64_t length);

		void unmap();

		void reallocate();

		void reallocate(const uint64_t byte_size);

		void upload(const std::vector<uint8_t>& data, const uint64_t offset = 0);
		
		void upload(const uint8_t* data, const uint64_t length, const uint64_t offset = 0);

		uint64_t size() const;

		bool is_mapped() const;

		bool is_synchronized() const;

	};

}
