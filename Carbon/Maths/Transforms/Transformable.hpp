#pragma once

#include <glm/glm.hpp>


#include "Translatable.hpp"
#include "Rotatable.hpp"
#include "Scalable.hpp"

#include "../Transform.hpp"
#include "../Matrix.hpp"
#include "../Maths.hpp"

#include "../../Control/Events/EventHost.hpp"

namespace cbn
{
	
	// Empty class which represents no transform
	struct NullTransform {};

	//TODO: use concepts to enforce T1,T2, and T3 to be the correct types.
	template<typename T1, typename T2 = NullTransform, typename T3 = NullTransform>
	class Transformable : public T1, public T2, public T3
	{
	public:

		EventHost<Transformable<T1, T2, T3>, const Transformable<T1, T2, T3>&> TransformEvent;

	private:

		mutable bool m_TransformOutdated, m_MatrixOutdated;
		mutable glm::mat4 m_TransformMatrixCache;
		mutable Transform m_TransformCache;
		bool m_DisableTransformEvent;

		template<typename T>
		static constexpr bool has_transform_type();

		std::tuple<glm::vec2, float, glm::vec2> resolve_transforms() const;

		void distribute_transforms_silent(const Transform& transform);

		void distribute_transforms(const Transform& transform);

		void initialize_cache_subscriptions();

		void on_translate();

		void on_rotate();

		void on_scale();

	protected:

		virtual void on_transform();
		
	public:

		Transformable();

		Transformable(const Transformable& copy);

		Transformable(const Transform& transform);

		void transform_to(const Transform& transform);

		void transform_by(const Transform& transform);

		const glm::mat4& transform_matrix() const;

		const Transform& as_transform() const;

	};
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	template<typename T>
	inline constexpr bool Transformable<T1, T2, T3>::has_transform_type()
	{
		return std::is_same_v<T1, T> || std::is_same_v<T2, T> || std::is_same_v<T3, T>;
	}

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline std::tuple<glm::vec2, float, glm::vec2> Transformable<T1, T2, T3>::resolve_transforms() const
	{
		// We want to resolve the translation, scale and rotation of the Transformable.
		// We could query for the type of T1, T2, T3 independently with constexpr ifs to
		// check which are translatables, scalables and rotatables respectively. However
		// we would have to account for the possible orders of the transformables.
		// Instead, since the Transformable inherits T1,T2, and T3; it means, for example,
		// that if one of T1,T2,T3 is a Translatable2D then we should be able to do
		// this->translation() to get the translation. So we can simplify the process
		// by instead just checking at least one of T1, T2, T3 is the required class
		// so that we can just use its API.
		
		// If the transform is translatable, get its translation
		glm::vec2 translation(0, 0);
		if constexpr(has_transform_type<Translatable2D>())
			translation = this->translation();

		// If the transform is rotatable, get its rotation
		float rotation_degrees = 0;
		if constexpr(has_transform_type<Rotatable2D>())
			rotation_degrees = this->rotation_degrees();

		// If the transform is either scalable1D or scalable2D get its scale
		// Note that the default scale in the case that the transformable is
		// not scalable, needs to be 1 so that nothing is scaled. 
		glm::vec2 scale(1, 1);
		if constexpr(has_transform_type<Scalable2D>())
			scale = this->scale();
		else if constexpr(has_transform_type<Scalable1D>())
			scale = {this->scale(), this->scale()};

		return {translation, rotation_degrees, scale};
	}


	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline void Transformable<T1, T2, T3>::distribute_transforms_silent(const Transform& transform)
	{
		// Here we want to use the same method used for resolving the transforms.
		// However we want to use the API to set the transforms instead.


		// Note that when any of the functions above are called to 
		// distribute a specific transform, they will call their
		// on_xxx method and set the cache outdated flags for us.
		// They would also dispatch the TransformEvent for us, so
		// we want to disable it temporarily so that we don't get
		// potentially 3 event dispatches for a single action.
		m_DisableTransformEvent = true;

		// If the transform is translatable, set its translation
		if constexpr(has_transform_type<Translatable2D>())
			this->translate_to(transform.translation());

		// If the transform is rotatable, set its rotation
		if constexpr(has_transform_type<Rotatable2D>())
			this->rotate_to(transform.rotation_degrees());

		// If the transform is either scalable1D or scalable2D set its scale
		if constexpr(has_transform_type<Scalable2D>())
			this->scale_to(transform.scale());
		else if constexpr(has_transform_type<Scalable1D>())
			this->scale_to(transform.scale().x);
	
		m_DisableTransformEvent = false;
	}

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline void Transformable<T1, T2, T3>::distribute_transforms(const Transform& transform)
	{
		distribute_transforms_silent(transform);

		TransformEvent.dispatch(*this);
	}

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline void Transformable<T1, T2, T3>::on_translate()
	{
		// If it is inherited, this is called by a Translatable when its translation changes
		m_TransformOutdated = true;
		m_MatrixOutdated = true;

		// This has to happen after the outdated flags are set
		// in case the user wants to use the transform or matrix.
		on_transform();

		if(!m_DisableTransformEvent)
			TransformEvent.dispatch(*this);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline void Transformable<T1, T2, T3>::on_rotate()
	{
		// If it is inherited, this is called by a rotatable when its rotation changes
		m_TransformOutdated = true;
		m_MatrixOutdated = true;

		// This has to happen after the outdated flags are set
        // in case the user wants to use the transform or matrix.
		on_transform();

		if(!m_DisableTransformEvent)
			TransformEvent.dispatch(*this);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline void Transformable<T1, T2, T3>::on_scale()
	{
		// If it is inherited, this is called by a scalable when its scale changes
		m_TransformOutdated = true;
		m_MatrixOutdated = true;
		
		// This has to happen after the outdated flags are set
		// in case the user wants to use the transform or matrix.
		on_transform();

		if(!m_DisableTransformEvent)
			TransformEvent.dispatch(*this);
	}

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline void Transformable<T1, T2, T3>::on_transform()
	{}
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline Transformable<T1, T2, T3>::Transformable()
		: m_TransformOutdated(true),
		  m_MatrixOutdated(true),
		  m_DisableTransformEvent(false)
	{}
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline Transformable<T1, T2, T3>::Transformable(const Transformable& copy)
		: Transformable()
	{
		distribute_transforms_silent(copy.as_transform());
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline Transformable<T1, T2, T3>::Transformable(const Transform& transform)
		: Transformable()
	{
		distribute_transforms_silent(transform);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline void Transformable<T1, T2, T3>::transform_to(const Transform& transform)
	{
		distribute_transforms(transform);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline void Transformable<T1, T2, T3>::transform_by(const Transform& transform)
	{
		transform_to(transform.apply(as_transform()));
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline const glm::mat4& Transformable<T1, T2, T3>::transform_matrix() const
	{
		if(m_MatrixOutdated)
		{
			const auto [translation, rotation_degrees , scale] = resolve_transforms();

			m_TransformMatrixCache = build_transform_matrix(translation, scale, to_radians(rotation_degrees));

			m_MatrixOutdated = false;
		}

		return m_TransformMatrixCache;
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline const Transform& Transformable<T1, T2, T3>::as_transform() const
	{
		if(m_TransformOutdated)
		{
			const auto [translation, rotation, scale] = resolve_transforms();

			m_TransformCache.translate_to(translation);
			m_TransformCache.rotate_to(rotation);
			m_TransformCache.scale_to(scale);

			m_TransformOutdated = false;
		}

		return m_TransformCache;
	}

	//-------------------------------------------------------------------------------------
}