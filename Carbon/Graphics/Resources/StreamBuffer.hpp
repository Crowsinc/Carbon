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
		
		static SRes<StreamBuffer> Allocate(const Buffer::Target target, const uint64_t byte_size, const bool unsynchronized, const Version opengl_version);

	private:
		
		const bool m_Unsynchronized;
		const GLbitfield m_MappingFlags;
		const uint64_t m_ByteSize;
		bool m_Mapped;

		StreamBuffer(const Buffer::Target target, const uint64_t byte_size, const bool unsynchronized);

	public:

		void* map();

		void unmap();

		uint64_t size() const;

		bool is_mapped() const;

		bool is_unsynchronized() const;

	};

}
