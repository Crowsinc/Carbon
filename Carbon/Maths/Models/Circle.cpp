#include "Circle.hpp"

#include <glm/gtx/projection.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace cbn
{
    //-------------------------------------------------------------------------------------

    Circle::Circle()
        : m_Radius(0)
    {}

    //-------------------------------------------------------------------------------------

    Circle::Circle(const float radius)
    {
        resize(radius);
    }

    //-------------------------------------------------------------------------------------

    Circle::Circle(const Transform& transform, const float radius)
    {
        resize(radius);
        this->transform_to(transform);
    }

    //-------------------------------------------------------------------------------------

    void Circle::resize(const float radius)
    {
        m_Radius = std::abs(radius);
    }

    //-------------------------------------------------------------------------------------

    float Circle::radius() const
    {
        return m_Radius;
    }

    //-------------------------------------------------------------------------------------

    const glm::vec2& Circle::centre() const
    {
        return translation();
    }

    //-------------------------------------------------------------------------------------
  
    Extent Circle::extent() const
    {
        const glm::vec2 xy_radius{m_Radius, m_Radius};

        return Extent{centre() - xy_radius, centre() + xy_radius};
    }

    //-------------------------------------------------------------------------------------
   
    template<>
    bool overlaps(const Circle& c1, const Circle& c2)
    {
        // Two circles overlap if their distance between centres 
        // is less than the sum of their radii
        const float r1 = c1.radius(), r2 = c2.radius();
        return glm::distance2(c1.centre(), c2.centre()) <= (r1 + r2) * (r1 + r2);
    }
    
    //-------------------------------------------------------------------------------------

    template<>
    bool overlaps(const Circle& c1, const Extent& c2)
    {
        // Re-use the extent-circle test
        return overlaps(c2, c1);
    }
    
    //-------------------------------------------------------------------------------------

    //template<>
    //bool overlaps(const Circle& c1, const Polygon& c2)
    //{
    //    // Re-use the polygon-circle test
    //    return overlaps(c2, c1);
    //}
    
    //-------------------------------------------------------------------------------------

    template<>
    bool overlaps(const Circle& c1, const Rectangle& c2)
    {
        // Re-use the rectangle-circle test
        return overlaps(c2, c1);
    }
    
    //-------------------------------------------------------------------------------------

    template<>
    bool contains(const Circle& c, const glm::vec2& p)
    {
        // The point is within the circle if its distance to the circle's centre is less than its radius
        const auto& r = c.radius();
        return glm::distance2(c.centre(), p) <= r * r;
    }

    //-------------------------------------------------------------------------------------

}