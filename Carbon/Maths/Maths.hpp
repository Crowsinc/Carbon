#pragma once

#include <math.h>
#include <tuple>
#include <glm/glm.hpp>

namespace cbn
{
	constexpr float PI = 3.14159265358979323846f;
	constexpr float PI_2 = PI / 2.0f;
	constexpr float PI_4 = PI / 4.0f;
	constexpr float PI_6 = PI / 6.0f;

	int next_power(int value, int base);

	unsigned next_power(unsigned value, unsigned base);

	float to_degrees(const float radians);

	float to_radians(const float degrees);

	float lerp(const float start, const float end, const float t);
	
	glm::vec2 lerp(const glm::vec2& start, const glm::vec2& end, const float t);

	float round_to_multiple(const float value, const float multiple);

	bool intersects(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& q1, const glm::vec2& q2);

	bool collinear(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

	bool on_line_segment(const glm::vec2& l1, const glm::vec2& l2, const glm::vec2& p);

}
