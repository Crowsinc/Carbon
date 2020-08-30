#pragma once

/*
		Vertex:

		8 ushorts per vertex => 16 bytes per vertex

		+

		4 bytes (RGBA) per vertex

		+

		4 byte per vertex for 4 samplers (packed)

		+

		2 floats per vertex => 8 bytes per vertex

		TOTAL: 32 bytes per vertex => 128 bytes per quad

		Use

		set(SLOT_ENUM, Key, Texture/Atlas);

	
	
	-----
		Rendering:

		Goal: 100k sprites of different textures at 100fps.


		TODO: Update AABB to take in a rect and be performed with a pre-transformed sprite. 
		That way we actually skip all the CPU costs of performing the transforms. 



		///

		- GL33 round robin buffers (or single buffer split)
		- GLmap unsynchronised, with my own fast synchronisation. 

		 
	
	*/

namespace cbn
{

	class SpriteRenderer
	{

	};

}