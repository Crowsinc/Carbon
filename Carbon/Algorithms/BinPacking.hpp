#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <functional>

#include "../Maths/Shapes.hpp"

namespace cbn
{
	enum class BinPackingHeuristic
	{
		BAF, // Best Area Fit
		BSSF, // Best Short Side Fit
		BLSF, // Best Long Side Fit
		BAF_BLSF, // Best Area Fit + Best Long Side Fit Tiebreaker.
		BAF_BSSF, // Best Area Fit + Best Short Side Fit Tiebreaker.
		BSSF_BAF, // Best Short Side Fit + Best Area Fit Tiebreaker
		BLSF_BAF, // Best Long Side Fit + Best Area Fit Tiebreaker
		BSSF_BLSF, // Best Short Side Fit + Best Long Side Fit Tiebreaker.
		BLSF_BSSF, // Best Long Side Fit + Best Short Side Fit Tiebreaker.
		
		BSSF_SQR, // Best Short Side Fit + Square Footprint
		BLSF_SQR, // Best Long Side Fit + Square Footprint
		BSSF_BAF_SQR, // Best Short Side Fit + Best Area Fit Tiebreaker + Square Footprint
		BLSF_BAF_SQR,  // Best Long Side Fit + Best Area Fit Tiebreaker + Square Footprint
		BSSF_BLSF_SQR, // Best Short Side Fit + Best Long Side Fit Tiebreaker + Square Footprint
		BLSF_BSSF_SQR, // Best Long Side Fit + Best Short Side Fit Tiebreaker + Square Footprint
	};

	bool max_rects_optimal(const glm::uvec2& size, const bool allow_rotation, const BinPackingHeuristic heuristic, std::vector<Rect<int>>& rectangles);

}