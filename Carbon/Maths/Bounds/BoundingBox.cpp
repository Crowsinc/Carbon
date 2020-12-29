#include "BoundingBox.hpp"

#include <algorithm>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "../Maths.hpp"
#include "../Matrix.hpp"
#include "../../Diagnostics/Assert.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	void BoundingBox::generate_local_mesh()
	{
		// The mesh is generated with (0,0) in its centre, and then offset by the local origin
		// That means if the local origin is (0,0), then the local origin will be in the centre
		// of the box, hence any rotation transforms will be about the centre of the box. While 
		// for example, if the origin was (-width/2, height/2), then the local origin would be 
		// the top left of the the box, hence rotations would go about the top left vertex. 
		// Similarly, the local centre of the box would now be (width/2, -height/2). 

		const auto half_size = m_Size * 0.5f;

		// Top left vertex
		m_LocalVertices.vertex_1.x = -half_size.x - m_LocalOrigin.x;
		m_LocalVertices.vertex_1.y =  half_size.y - m_LocalOrigin.y;
	
		// Bottom left vertex
		m_LocalVertices.vertex_2.x = -half_size.x - m_LocalOrigin.x;
		m_LocalVertices.vertex_2.y = -half_size.y - m_LocalOrigin.y;

		// Bottom right vertex
		m_LocalVertices.vertex_3.x =  half_size.x - m_LocalOrigin.x;
		m_LocalVertices.vertex_3.y = -half_size.y - m_LocalOrigin.y;

		// Top right vertex
		m_LocalVertices.vertex_4.x =  half_size.x - m_LocalOrigin.x;
		m_LocalVertices.vertex_4.y =  half_size.y - m_LocalOrigin.y;
	
		// Set the local centre
		m_LocalCentre = m_LocalOrigin;
	}

	//-------------------------------------------------------------------------------------

	BoundingBox::BoundingBox(const glm::vec2& size, const glm::vec2& local_origin)
	{
		resize(size, local_origin);
	}
	
	//-------------------------------------------------------------------------------------

	BoundingBox::BoundingBox(const cbn::Transform& transform, const glm::vec2& size, const glm::vec2& local_origin)
		: Transformable(transform)
	{
		resize(size, local_origin);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::overlaps(const BoundingArea& area) const
	{
		return area.overlaps(*this);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::overlaps(const BoundingBox& box) const
	{
		// If both boxes are very close to being axis alligned, then 
		// treat them as axis alligned bounding boxes to speed up
		// the calculation.
		const auto allignment_2 = std::fmodf(box.rotation_degrees(), 90);

		if(is_axis_alligned() && box.is_axis_alligned())
		{
			// As both boxes are (or are close to) being axis alligned, we can just
			// find their minimum and maximum coords and use that for the calculation.
			// This method is by far faster than performing SAT. 
			const auto [min_x_1, max_x_1, min_y_1, max_y_1] = find_min_max_coords();
			const auto [min_x_2, max_x_2, min_y_2, max_y_2] = box.find_min_max_coords();

			const bool x_axis_overlap = (max_x_1 >= min_x_2) && (min_x_1 <= max_x_2);
			const bool y_axis_overlap = (max_y_1 >= min_y_2) && (min_y_1 <= max_y_2);

			return x_axis_overlap && y_axis_overlap;
		}
		else
		{
			// If we cannot do an axis alligned collision check, then we have to use separating axis theorem
			// Since we are checking for overlap on boxes, which have two edges that are parallel. We can skip
			// duplicate parallel edges, meaning out of the 8 total edges we only need to check 4. 

			// Based on: https://jcharry.com/blog/physengine10

			const auto box_1_mesh = vertices();
			const auto box_2_mesh = box.vertices();

			// Find the two non-parallel edges of each box
			const glm::vec2 box_1_edge_1 = box_1_mesh.vertex_2 - box_1_mesh.vertex_1;
			const glm::vec2 box_1_edge_2 = box_1_mesh.vertex_4 - box_1_mesh.vertex_1;

			const glm::vec2 box_2_edge_1 = box_2_mesh.vertex_2 - box_2_mesh.vertex_1;
			const glm::vec2 box_2_edge_2 = box_2_mesh.vertex_4 - box_2_mesh.vertex_1;

			// Get the counter-clockwise normals to each of these edges and store them in an array.
			// We also want to make them unit vectors in order to speed up some calculations later. 
			const std::array<glm::vec2, 4> axes = {
				glm::normalize(glm::vec2{-box_1_edge_1.y, box_1_edge_1.x}),
				glm::normalize(glm::vec2{-box_1_edge_2.y, box_1_edge_2.x}),
				glm::normalize(glm::vec2{-box_2_edge_1.y, box_2_edge_1.x}),
				glm::normalize(glm::vec2{-box_2_edge_2.y, box_2_edge_2.x})
			};
			
			const auto box_1_verts = box_1_mesh.as_array();
			const auto box_2_verts = box_2_mesh.as_array();
			std::array<float, 4> box_1_projections;
			std::array<float, 4> box_2_projections;

			// Perform intersection tests on all normal axes
			for(const auto axis : axes)
			{
				// Find the scalar projections of each box vertex to each axis.
				// Note that since we normalized the axes, we can simplify the scalar projection to a dot product. 
				
				std::transform(box_1_verts.begin(), box_1_verts.end(), box_1_projections.begin(), [&](const glm::vec2& v){
					return glm::dot(v, axis);
				});

				std::transform(box_2_verts.begin(), box_2_verts.end(), box_2_projections.begin(), [&](const glm::vec2& v)
				{
					return glm::dot(v, axis);
				});

				const auto [min_1, max_1] = std::minmax_element(box_1_projections.begin(), box_1_projections.end());
				const auto [min_2, max_2] = std::minmax_element(box_2_projections.begin(), box_2_projections.end());

				// Check if there is any overlap between the box edges on this axis
				const float overlap = std::fminf(*max_1, *max_2) - std::fmaxf(*min_1, *min_2);

				// If there is no or negative overlap, then the boxes cannot be overlapping so return false early
				if(overlap <= 0)
					return false;

			}

			// If we made it this far, then there was overlap on all axes, so return true
			return true;
		}
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::overlaps(const BoundingCircle& circle) const
	{
		return circle.overlaps(*this);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::encloses(const BoundingBox& box) const
	{
		// This bounding box encloses the other if all its vertices are within this one
		const auto verts = box.vertices();
		return contains_point(verts.vertex_1) && contains_point(verts.vertex_2) 
			&& contains_point(verts.vertex_3) && contains_point(verts.vertex_4);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::encloses(const BoundingCircle& circle) const
	{
		// This bounding box encloses the circle if they overlap and the 
		// circle is not intersected by any of the box's faces
		
		const auto verts = vertices();

		// This may not be very fast, but its simple
		return circle.overlaps(*this) 
			&& !circle.is_intersected_by_line(verts.vertex_1, verts.vertex_2)
		    && !circle.is_intersected_by_line(verts.vertex_2, verts.vertex_3)
		    && !circle.is_intersected_by_line(verts.vertex_3, verts.vertex_4)
		    && !circle.is_intersected_by_line(verts.vertex_4, verts.vertex_1);
	}
	
	//-------------------------------------------------------------------------------------
	
	bool BoundingBox::contains_point(const glm::vec2& point) const
	{
		// Solution taken from: https://math.stackexchange.com/a/190373
		// It basically involves using dot products between vectors from
		// vertices to the point, and vectors forming the edges of the rectangle
		// to check that the angle between the vectors is such that the point is inside
		// the rectangle. Remember, if the dot product is 0 it means that vectors are normal,
		// if its >0, then they point in roughly the same direction, and if its <0 they point
		// in roughly opposite directions. 

		const auto verts = vertices();

		// Since we can't name the vectors with numbers, 
		// P will be the point, then the vertices are A-D from v1-v4

		const glm::vec2 AP = point - verts.vertex_1;
		const glm::vec2 AD = verts.vertex_4 - verts.vertex_1;
		const glm::vec2 DP = point - verts.vertex_4;
		const glm::vec2 AB = verts.vertex_2 - verts.vertex_1;

		return glm::dot(AP, AD) > 0 && glm::dot(DP, AD) < 0 && glm::dot(AP, AD) > 0 
			&& glm::dot(AD,AD) > glm::dot(AP, AD);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::is_intersected_by_line(const glm::vec2& p1, const glm::vec2& p2) const
	{
		// Simply check if the end points are in the box, or the line intersects any of the sides of the box

		if(contains_point(p1) || contains_point(p2))
			return true;

		const auto verts = vertices();

		return intersects(verts.vertex_1, verts.vertex_2, p1, p2)
			|| intersects(verts.vertex_2, verts.vertex_3, p1, p2)
			|| intersects(verts.vertex_3, verts.vertex_4, p1, p2)
			|| intersects(verts.vertex_4, verts.vertex_1, p1, p2);
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::is_intersected_by_ray(const glm::vec2& origin, const glm::vec2& towards) const
	{
		// The ray intersects the box if the origin is in the box, or the direction of the ray 
		// falls in line between the direction from the origin to the box. 

		// If the box contains the origin, exit early with true
		if(contains_point(origin))
			return true;

		// Otherwise, we need to determine the angles between each of the vertices of 
		// the box and the origin as well as the angle of the direction. 

		const auto verts = vertices();

		const auto dir_angle = std::atan2f(towards.y - origin.y, towards.x - origin.x);
		const auto v1_angle = std::atan2f(verts.vertex_1.y - origin.y, verts.vertex_1.x - origin.x);
		const auto v2_angle = std::atan2f(verts.vertex_2.y - origin.y, verts.vertex_2.x - origin.x);
		const auto v3_angle = std::atan2f(verts.vertex_3.y - origin.y, verts.vertex_3.x - origin.x);
		const auto v4_angle = std::atan2f(verts.vertex_4.y - origin.y, verts.vertex_4.x - origin.x);

		const auto max_angle = std::fmaxf(v1_angle, std::fmaxf(v2_angle, std::fmaxf(v3_angle, v4_angle)));
		const auto min_angle = std::fminf(v1_angle, std::fminf(v2_angle, std::fminf(v3_angle, v4_angle)));

		return dir_angle <= max_angle && dir_angle >= min_angle;
	}

	//-------------------------------------------------------------------------------------

	BoundingCircle BoundingBox::wrap_as_bounding_circle() const
	{
		// Place the circle in the center of the bounding box and make its diameter be the
		// length of the diagonal of the bounding box. This will ensure that the 
		// box is inscribed within the circle. 
		
		//TODO: switch to fast flm sqrtf?
		const float diagonal = std::sqrtf(m_Size.x * m_Size.x + m_Size.y * m_Size.y);

		return {{}, diagonal / 2}; //TODO: {as_transform(), diagonal / 2}; -------------------------------------------------------------------------
	}
	
	//-------------------------------------------------------------------------------------

	BoundingBox BoundingBox::wrap_as_bounding_box() const
	{
		return *this;
	}
	
	//-------------------------------------------------------------------------------------

	void BoundingBox::set_local_origin(const glm::vec2& local_origin)
	{
		m_LocalOrigin = local_origin;

		generate_local_mesh();
	}

	//-------------------------------------------------------------------------------------

	std::tuple<float, float, float, float> BoundingBox::find_min_max_coords() const
	{
		// To find the minimum and maximum x and y coords, we can just make use of the 
		// rotation of the bounding box along with the properties of a rectangle. Depending on which 
		// quadrant the box is facing towards, we can instantly determine which vertex of the box 
		// will be the topmost, leftmost, rightmost and bottommost. We then use these coords to build 
		// an axis alligned box mesh where vertex 1 is top left, vertex 2 is bottom left, vertex 3 is 
		// bottom right, and vertex 4 is top right

		const auto verts = vertices();
		const auto rotation = rotation_degrees();

		float min_x, max_x, min_y, max_y;

		if(rotation >= 0 && rotation < 90)
		{
			// vertex 1 is top, vertex 2 is left, vertex 3 is bottom, vertex 4 is right
			max_x = verts.vertex_4.x;
			min_x = verts.vertex_2.x;
			max_y = verts.vertex_1.y;
			min_y = verts.vertex_3.y;
		}
		else if(rotation >= 90 && rotation < 180)
		{
			// vertex 1 is right, vertex 2 is top, vertex 3 is left, vertex 4 is bottom
			max_x = verts.vertex_1.x;
			min_x = verts.vertex_3.x;
			max_y = verts.vertex_2.y;
			min_y = verts.vertex_4.y;
		}
		else if(rotation >= 180 && rotation < 270)
		{
			// vertex 1 is bottom, vertex 2 is right, vertex 3 is top, vertex 4 is left
			max_x = verts.vertex_2.x;
			min_x = verts.vertex_4.x;
			max_y = verts.vertex_3.y;
			min_y = verts.vertex_1.y;
		}
		else if(rotation >= 270 && rotation < 360)
		{
			// vertex 1 is left, vertex 2 is bottom, vertex 3 is right, vertex 4 is top
			max_x = verts.vertex_3.x;
			min_x = verts.vertex_1.x;
			max_y = verts.vertex_4.y;
			min_y = verts.vertex_2.y;
		}
		else CBN_Assert(false, "The rotation is not wrapping between 0 and 360 as expected");

		return {min_x, max_x, min_y, max_y};
	}

	//-------------------------------------------------------------------------------------

	BoundingBox BoundingBox::wrap_as_axis_alligned() const
	{
		// To wrap the box in another axis alligned box, we first need to find its min/max x and y coords.
		const auto [min_x, max_x, min_y, max_y] = find_min_max_coords();

		// The size will now simply be the different between the min and max coords
		const glm::vec2 aabb_size = {max_x - min_x, max_y - max_y};

		// The transform will have the same translation as this one, but with no rotation or scaling
		Transform aabb_transform{translation()};

		// The aabb should have the same origin, we can re-use it because the origin is independent
		// from the size of the box, hence it doesnt matter that the aabb is a different size. 
		return {aabb_transform, aabb_size, m_LocalOrigin};
	}
	
	//-------------------------------------------------------------------------------------

	bool BoundingBox::is_axis_alligned(const float threshold) const
	{
		CBN_Assert(threshold >= 0, "Threshold cannot be negative");
		
		// It is axis alligned if the rotation is a multiple of 90 degrees. 
		// Since we are using floats, we provide a threshold to where the rotation
		// can be 'close enough' to axis alligned. 
		return std::fabsf(std::fmodf(rotation_degrees(), 90)) <= threshold;
	}

	//-------------------------------------------------------------------------------------

	void BoundingBox::resize(const glm::vec2& size, const glm::vec2& local_origin)
	{
		CBN_Assert(size.x > 0 && size.y > 0, "Size cannot be negative or 0");

		m_Size = size;
		m_LocalOrigin = local_origin;

		generate_local_mesh();
	}
	
	//-------------------------------------------------------------------------------------

	void BoundingBox::allign_to_axis() 
	{
		// To allign the vertices to the cartesian axes, we just round the rotation 
	    // of the bounding box to the nearest multiple of 90 degrees. 
		rotate_to(round_to_multiple(rotation_degrees(), 90));
	}

	//-------------------------------------------------------------------------------------

	const BoxMesh& BoundingBox::local_vertices() const
	{
		return m_LocalVertices;
	}
	
	//-------------------------------------------------------------------------------------

	glm::vec2 BoundingBox::local_centre() const
	{
		// Centre relative to a mesh which where its origin is at (0,0)
		return m_LocalCentre;
	}
	
	//-------------------------------------------------------------------------------------

	glm::vec2 BoundingBox::local_origin() const
	{
		// Origin relative to a mesh which is centered at (0,0)
		return m_LocalOrigin;
	}
	
	//-------------------------------------------------------------------------------------

	void BoundingBox::TEMP_UPDATE_CACHE() const
	{
		const auto& matrix = transform_matrix();

		m_CachedVertices = {
			transform(m_LocalVertices.vertex_1, matrix),
			transform(m_LocalVertices.vertex_2, matrix),
			transform(m_LocalVertices.vertex_3, matrix),
			transform(m_LocalVertices.vertex_4, matrix)
		};
	}
	
	//-------------------------------------------------------------------------------------

	const BoxMesh& BoundingBox::vertices() const
	{
		return m_CachedVertices;
	}
	
	//-------------------------------------------------------------------------------------

	glm::vec2 BoundingBox::centre() const
	{
		const auto& matrix = transform_matrix();

		return transform(m_LocalCentre, matrix);
	}
	
	//-------------------------------------------------------------------------------------

	glm::vec2 BoundingBox::size() const
	{
		return m_Size;
	}
	
	//-------------------------------------------------------------------------------------

	glm::vec2 BoundingBox::scaled_size() const
	{
		return m_Size * scale();
	}
	
	//-------------------------------------------------------------------------------------

	std::array<glm::vec2, 4> BoxMesh::as_array() const
	{
		return {vertex_1, vertex_2, vertex_3, vertex_4};
	}
	
	//-------------------------------------------------------------------------------------

}