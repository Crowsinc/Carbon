#pragma once

#include <span>
#include <math.h>
#include <glm/glm.hpp>
#include <algorithm>

namespace cbn
{
    bool test_separating_axis_theorem(
        const std::span<const glm::vec2>& axes,
        const std::span<const glm::vec2>& vertices_1,
        const std::span<const glm::vec2>& vertices_2
    );

    bool test_separating_axis_theorem(
        const std::span<const glm::vec2>& axes_1,
        const std::span<const glm::vec2>& axes_2,
        const std::span<const glm::vec2>& vertices_1,
        const std::span<const glm::vec2>& vertices_2
    );

}