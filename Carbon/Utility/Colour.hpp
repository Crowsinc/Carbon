#pragma once

#include <stdint.h>

namespace cbn
{
	union Colour;
	
	struct HSLColour
	{
		float hue;
		float saturation;
		float luminance;

		Colour to_rgba() const;
	};

#pragma pack(push, 1)

	union Colour
	{
		static constexpr int Components();

		uint32_t hex;
		struct
		{
			uint8_t red;
			uint8_t green;
			uint8_t blue;
			uint8_t alpha;
		};

		Colour();

		Colour(const uint32_t hex);

		Colour(const Colour& rgba);

		Colour(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha = 255);

		bool operator==(const Colour other) const;

		bool operator==(const uint32_t hex) const;

		HSLColour to_hsl() const;

	};

#pragma pack(pop)

	const Colour White = {255, 255, 255};
	const Colour Black = {0, 0, 0};
	const Colour Red = {255, 0, 0};
	const Colour Green = {0, 128, 0};
	const Colour Blue = {0, 0, 255};
	const Colour Yellow = {255, 255,0};
	const Colour Purple = {128, 0, 128};
	const Colour Magenta = {255, 0, 255};
	const Colour Orange = {255, 165, 0};
	const Colour Lime = {0, 255, 0};
	const Colour Cyan = {0, 255, 255};
	const Colour Brown = {139, 69, 19};
	const Colour Indigo = {75, 0, 130};

}

