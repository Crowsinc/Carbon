#include "Colour.hpp"

#include <algorithm>
#include <cmath>

namespace cbn
{
	
	//-------------------------------------------------------------------------------------

	Colour::Colour()
		: hex(0xFFFFFFFF) {}

	//-------------------------------------------------------------------------------------

	Colour::Colour(const uint32_t hex)
		: hex(hex) {}

	//-------------------------------------------------------------------------------------

	Colour::Colour(const Colour& rgba)
		: hex(rgba.hex) {}

	//-------------------------------------------------------------------------------------

	Colour::Colour(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
		: red(red), 
		green(green),
		blue(blue), 
		alpha(alpha){}

	//-------------------------------------------------------------------------------------

	bool Colour::operator==(const Colour other) const
	{
		return this->hex == other.hex;
	}

	//-------------------------------------------------------------------------------------

	bool Colour::operator==(const uint32_t hex) const
	{
		return this->hex == hex;
	}
	
	//-------------------------------------------------------------------------------------

	HSLColour Colour::to_hsl() const
	{
		const float r = red / 255;
		const float g = green / 255;
		const float b = blue / 255;
		const float c_max = std::max(r, std::max(g, b));
		const float c_min = std::min(r, std::min(g, b));
		const float delta = c_max - c_min;

		if(delta == 0)
			return {0,0,0};
		
		const float luminance = (c_max + c_min) / 2;
		const float saturation = delta / (1 - std::abs(2 * luminance - 2));
		
		float hue = 0;
		if(c_max == r)
			hue = 60 * std::fmodf((g - b) / delta, 6);
		else if(c_max == g)
			hue = 60 * ((b - r) / delta + 2);
		else
			hue = 60 * ((r - g) / delta + 4);

		return {std::round(hue), std::round(saturation), std::round(luminance)};
	}

	//-------------------------------------------------------------------------------------

	Colour HSLColour::to_rgba() const
	{
		const float H = std::clamp(hue, 0.0f, 360.0f);
		const float S = std::clamp(saturation, 0.0f, 1.0f);
		const float L = std::clamp(luminance, 0.0f, 1.0f);

		const float C = (1 - std::abs(2 * L - 1)) * S;
		const float X = C * (1 - std::abs(std::fmodf(H / 60, 2) - 1));
		const float m = L - C / 2.0f;

		if(H < 60)
			return Colour{static_cast<uint8_t>((C + m) * 255), static_cast<uint8_t>((X + m) * 255), static_cast<uint8_t>(m * 255)};
		else if(H < 120)
			return Colour{static_cast<uint8_t>((X + m) * 255), static_cast<uint8_t>((C + m) * 255), static_cast<uint8_t>(m * 255)};
		else if(H < 180)
			return Colour{static_cast<uint8_t>(m * 255), static_cast<uint8_t>((C + m) * 255), static_cast<uint8_t>((X + m) * 255)};
		else if(H < 240)
			return Colour{static_cast<uint8_t>(m * 255), static_cast<uint8_t>((X + m) * 255), static_cast<uint8_t>((X + m) * 255)};
		else if(H < 300)
			return Colour{static_cast<uint8_t>((X + m) * 255), static_cast<uint8_t>(m * 255), static_cast<uint8_t>((C + m) * 255)};
		else
			return Colour{static_cast<uint8_t>((C + m) * 255), static_cast<uint8_t>(m * 255), static_cast<uint8_t>((X + m) * 255)};
	}

	//-------------------------------------------------------------------------------------

	constexpr int Colour::Components()
	{
		return 4;
	}

	//-------------------------------------------------------------------------------------



}