#include "SeparatingAxisTheorem.hpp"

#include <vector>

#include "../Diagnostics/Assert.hpp"

namespace cbn
{

    //-------------------------------------------------------------------------------------

    bool test_separating_axis_theorem(
        const std::span<const glm::vec2>& axes_1,
        const std::span<const glm::vec2>& axes_2,
        const std::span<const glm::vec2>& vertices_1,
        const std::span<const glm::vec2>& vertices_2
    )
    {
        return test_separating_axis_theorem(axes_1, vertices_1, vertices_2) && test_separating_axis_theorem(axes_2, vertices_1, vertices_2);
    }
    
    //-------------------------------------------------------------------------------------

    bool test_separating_axis_theorem(
        const std::span<const glm::vec2>& axes,
        const std::span<const glm::vec2>& vertices_1,
        const std::span<const glm::vec2>& vertices_2
    )
    {
        std::vector<float> projections_1(vertices_1.size()), projections_2(vertices_2.size());

        for (auto a = 0; a < axes.size(); a++)
        {
            const auto& axis = axes[a];

            // Ensure that the axis is a unit vector as we can optimise projections by only doing dot products
            CBN_Assert(glm::length(axis) == 1.0f, "Axis must be a unit vector");

            for (auto i = 0; i < vertices_1.size(); i++)
                projections_1[i] = glm::dot(axis, vertices_1[i]);

            for (auto i = 0; i < vertices_2.size(); i++)
                projections_2[i] = glm::dot(axis, vertices_2[i]);

            const auto [min_1, max_1] = std::minmax_element(projections_1.begin(), projections_1.end());
            const auto [min_2, max_2] = std::minmax_element(projections_2.begin(), projections_2.end());

            if (*max_1 < *min_2 || *min_1 > *max_2)
                return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------------------

}