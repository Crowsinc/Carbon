#pragma once

#include <math.h>
#include <tuple>
#include <glm/glm.hpp>
#include <algorithm>

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

	bool segments_intersect(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& q1, const glm::vec2& q2);

	bool collinear(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

	int signum(const float v);

	int line_side(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

	bool on_segment(const glm::vec2& l1, const glm::vec2& l2, const glm::vec2& p);

	glm::vec2 closest_segment_point(const glm::vec2& l1, const glm::vec2& l2, const glm::vec2& p);

	bool ray_segment_intersection(const glm::vec2& origin, const glm::vec2& towards, const glm::vec2& l1, const glm::vec2& l2);
	

	// TODO: clean up
	template<unsigned A, unsigned V1, unsigned V2>
	bool sat_test(const std::array<glm::vec2, A>& unit_axes, const std::array<glm::vec2, V1>& poly_1, const std::array<glm::vec2, V2>& poly_2);

}

//TODO: clean up and optimise. 
template<unsigned A, unsigned V1, unsigned V2>
bool cbn::sat_test(const std::array<glm::vec2, A>& unit_axes, const std::array<glm::vec2, V1>& poly_1, const std::array<glm::vec2, V2>& poly_2)
{
	std::array<float, V1> poly_1_projections = {};
	std::array<float, V2> poly_2_projections = {};

	// Perform intersection tests on all unit axes
	for(auto a = 0; a < A; a++)
	{
		const auto axis = unit_axes[a];

		// Find the scalar projections of each polygon vertex to each axis.
		// Since the axes are unit vectors we can simplify the scalar projection to a dot product
		// which speeds things up as we do 16 projections but only 4 normalizations. 

		for(auto i = 0; i < V1; i++)
			poly_1_projections[i] = glm::dot(axis, poly_1[i]);

		for(auto i = 0; i < V2; i++)
			poly_2_projections[i] = glm::dot(axis, poly_2[i]);

		const auto [min_1, max_1] = std::minmax_element(poly_1_projections.begin(), poly_1_projections.end());
		const auto [min_2, max_2] = std::minmax_element(poly_2_projections.begin(), poly_2_projections.end());

		if(*max_1 < *min_2 || *min_1 > *max_2)
			return false;
	}

	return true;
}
