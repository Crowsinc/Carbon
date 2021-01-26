#pragma once

#include "../Maths/Models/BoundingBox.hpp"
#include "../Maths/Transform.hpp"
#include "../Maths/Matrix.hpp"

namespace cbn
{


	class Camera : public Transformable<Translatable2D, Rotatable2D, Scalable2D>
	{
	private:

		glm::vec2 m_Resolution;

		mutable bool m_ProjectionMatrixOutdated, m_ViewMatrixOutdated, m_ViewProjectionMatrixOutdated;
		mutable glm::mat4 m_ViewMatrix, m_ProjectionMatrix, m_ViewProjectionMatrix;
		mutable bool m_BoundingBoxOutdated;
		mutable BoundingBox m_BoundingBox;

		void on_transform() override;

	public:
		
		Camera(const glm::vec2& resolution);

		Camera(const Transform& transform, const glm::vec2& resolution);

		void resize(const glm::vec2& resolution);

		const glm::vec2& resolution() const;
	
		const glm::mat4& view_matrix() const;

		const glm::mat4& projection_matrix() const;

		const glm::mat4& view_projection_matrix() const;

		const BoundingBox& bounding_box() const;
	};

}