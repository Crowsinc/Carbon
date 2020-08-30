#pragma once

#include <vector>

#include "../../Utility/Version.hpp"
#include "../../Memory/Resource.hpp"
#include "../OpenGL/Buffer.hpp"
#include "../OpenGL/OpenGL.hpp"

namespace cbn
{

	class StreamBuffer : Buffer
	{
	public:
		
		static SRes<StreamBuffer> Allocate(const Buffer::Target target, const uint64_t byte_size, const bool unsynchronized, const Version opengl_version);

	private:
		
		uint64_t m_MappedOffset, m_MappedLength;
		const bool m_Unsynchronized;
		GLbitfield m_MappingFlags;
		const uint64_t m_ByteSize;
		const bool m_Persistent;
		void* m_MappedPtr;
		bool m_IsMapped;

		StreamBuffer(const Buffer::Target target, const uint64_t byte_size, const bool unsynchronized, const bool persistent);

	public:

		void* map(const uint64_t offset = 0, const uint64_t length = 0);

		void unmap();

		uint64_t size() const;

		bool is_mapped() const;

		bool is_persistent() const;

		bool is_unsynchronized() const;

	};

}
