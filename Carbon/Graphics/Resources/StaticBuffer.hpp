#pragma once

#include <unordered_map>
#include <stdint.h>
#include <vector>

#include "../../Utility/Version.hpp"
#include "../../Memory/Resource.hpp"
#include "../OpenGL/Buffer.hpp"

namespace cbn
{

	class StaticBuffer : public Buffer
	{
	public:

		static SRes<StaticBuffer> Allocate(const std::vector<uint8_t>& data, const Buffer::Target target, const Version opengl_version);

	private:

		const uint64_t m_ByteSize;

		StaticBuffer(const Buffer::Target target, const uint64_t byte_size);

	public:

		uint64_t size() const;

	};

}