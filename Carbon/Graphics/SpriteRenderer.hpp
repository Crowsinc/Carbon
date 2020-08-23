#pragma once

/*
		Vertex:

		8 ushorts per vertex => 16 bytes per vertex

		+

		4 bytes (RGB) per vertex

		+

		4 byte per vertex for 4 samplers (packed)

		+

		2 floats per vertex => 8 bytes per vertex

		TOTAL: 32 bytes per vertex => 128 bytes per quad

		Use

		set(SLOT_ENUM, Key, Texture/Atlas);

	*/