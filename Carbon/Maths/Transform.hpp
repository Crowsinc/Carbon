#pragma once

#include <tuple>
#include <memory>
#include <iostream>
#include <glm/glm.hpp>

#include "Transforms/Scalable.hpp"
#include "Transforms/Rotatable.hpp"
#include "Transforms/Translatable.hpp"


#include "Maths.hpp"
#include "Matrix.hpp"

namespace cbn
{
	// TODO: clean this up + caching

	class Transform;

	struct NullTransform {};

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	glm::vec2 resolve_translation(const T1& t1, const T2& t2, const T3& t3);

	template<typename T1, typename T2, typename T3>
	glm::vec2 resolve_scale(const T1& t1, const T2& t2, const T3& t3);

	template<typename T1, typename T2, typename T3>
	float resolve_rotation(const T1& t1, const T2& t2, const T3& t3);

	template<typename T1, typename T2, typename T3>
	void distribute_translation(const Transform& transform, const T1& t1, const T2& t2, const T3& t3);

	template<typename T1, typename T2, typename T3>
	void distribute_scale(const Transform& transform, const T1& t1, const T2& t2, const T3& t3);

	template<typename T1, typename T2, typename T3>
	void distribute_rotation(const Transform& transform, const T1& t1, const T2& t2, const T3& t3);

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2 = NullTransform, typename T3 = NullTransform>
	class Transformable : public T1, public T2, public T3
	{
	private:


		virtual void on_update() {}

		//-------------------------------------------------------------------------------------

		std::tuple<glm::vec2, glm::vec2, float> resolve_transforms() const;

		void distribute_transforms(const Transform& transform);

		//-------------------------------------------------------------------------------------

	public:

		Transformable();

		Transformable(const Transformable& copy);
		
		Transformable(const Transform& transform);

		glm::mat4 transform_matrix() const;

		Transform as_transform() const;

	};
	
	//-------------------------------------------------------------------------------------

	class Transform : public Transformable<Translatable2D, Rotatable2D, Scalable2D>
	{
	public:

		Transform();
		
		Transform(const Transform& copy);

		Transform(const float x, const float y, const float rotation_degrees = 0, const float scale_x = 1, const float scale_y = 1);

		Transform(const glm::vec2& translation, const float rotation_degrees = 0, const glm::vec2& scale = {1, 1});

		glm::vec2 apply(const glm::vec2& point) const;

		Transform apply(const Transform& other) const;

	};

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline std::tuple<glm::vec2, glm::vec2, float> cbn::Transformable<T1, T2, T3>::resolve_transforms() const
	{
		const T1* t1 = this;
		const T2* t2 = this;
		const T3* t3 = this;

		return {
			resolve_translation(t1, t2, t3), 
			resolve_scale(t1, t2, t3), 
			resolve_rotation(t1, t2, t3)
		};
	}

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline glm::vec2 resolve_translation(const T1* t1, const T2* t2, const T3* t3)
	{
		// Translatable 2D
		if constexpr(std::is_same_v<T1, Translatable2D>)
			return t1->translation();
		else if constexpr(std::is_same_v<T2, Translatable2D>)
			return t2->translation();
		else if constexpr(std::is_same_v<T3, Translatable2D>)
			return t3->translation();
		else
			return {0,0};
	}

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline glm::vec2 resolve_scale(const T1* t1, const T2* t2, const T3* t3)
	{
		// Scalable 2D
		if constexpr(std::is_same_v<T1, Scalable2D>)
			return t1->scale();
		else if constexpr(std::is_same_v<T2, Scalable2D>)
			return t2->scale();
		else if constexpr(std::is_same_v<T3, Scalable2D>)
			return t3->scale();

		// Scalable 1D
		else if constexpr(std::is_same_v<T1, Scalable1D>)
			return {t1->scale(), t1->scale()};
		else if constexpr(std::is_same_v<T2, Scalable1D>)
			return {t2->scale(), t2->scale()};
		else if constexpr(std::is_same_v<T3, Scalable1D>)
			return {t3->scale(), t3->scale()};
		else 
			return {1.0f, 1.0f};
	}

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline float resolve_rotation(const T1* t1, const T2* t2, const T3* t3)
	{
		// Rotatable 2D
		if constexpr(std::is_same_v<T1, Rotatable2D>)
			return t1->rotation_degrees();
		else if constexpr(std::is_same_v<T2, Rotatable2D>)
			return t2->rotation_degrees();
		else if constexpr(std::is_same_v<T3, Rotatable2D>)
			return t3->rotation_degrees();
		else
			return 0;
	}

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline void cbn::Transformable<T1, T2, T3>::distribute_transforms(const Transform& transform)
	{
		T1* t1 = this;
		T2* t2 = this;
		T3* t3 = this;

		distribute_translation(transform, t1, t2, t3);
		distribute_rotation(transform, t1, t2, t3);
		distribute_scale(transform, t1, t2, t3);
	}

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline void distribute_translation(const Transform& transform, T1* t1, T2* t2, T3* t3)
	{
		// Translatable 2D
		if constexpr(std::is_same_v<T1, Translatable2D>)
			t1->translate_to(transform.translation());
		else if constexpr(std::is_same_v<T2, Translatable2D>)
			t2->translate_to(transform.translation());
		else if constexpr(std::is_same_v<T3, Translatable2D>)
			t3->translate_to(transform.translation());
		else {}
	}

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline void distribute_scale(const Transform& transform, T1* t1, T2* t2, T3* t3)
	{
		// Scalable 2D
		if constexpr(std::is_same_v<T1, Scalable2D>)
			t1->scale_to(transform.scale());
		else if constexpr(std::is_same_v<T2, Scalable2D>)
			t2->scale_to(transform.scale());
		else if constexpr(std::is_same_v<T3, Scalable2D>)
			t3->scale_to(transform.scale());

		// Scalable 1D
		else if constexpr(std::is_same_v<T1, Scalable1D>)
			t1->scale_to(transform.scale().x);
		else if constexpr(std::is_same_v<T2, Scalable1D>)
			t2->scale_to(transform.scale().x);
		else if constexpr(std::is_same_v<T3, Scalable1D>)
			t3->scale_to(transform.scale().x);
		else {}
	}

	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline void distribute_rotation(const Transform& transform, T1* t1, T2* t2, T3* t3)
	{
		// Rotatable 2D
		if constexpr(std::is_same_v<T1, Rotatable2D>)
			t1->rotate_to(transform.rotation_degrees());
		else if constexpr(std::is_same_v<T2, Rotatable2D>)
			t2->rotate_to(transform.rotation_degrees());
		else if constexpr(std::is_same_v<T3, Rotatable2D>)
			t3->rotate_to(transform.rotation_degrees());
		else {}
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline cbn::Transformable<T1, T2, T3>::Transformable() {} 
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline Transformable<T1, T2, T3>::Transformable(const Transformable& copy)
	{
		distribute_transforms(copy.as_transform());
	} 
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline cbn::Transformable<T1, T2, T3>::Transformable(const Transform& transform)
	{
		distribute_transforms(transform);
	}
	
	//-------------------------------------------------------------------------------------

	template<typename T1, typename T2, typename T3>
	inline glm::mat4 cbn::Transformable<T1, T2, T3>::transform_matrix() const
	{
		const auto [translation, scale, rotation] = resolve_transforms();
		return build_transform_matrix(translation, scale, to_radians(rotation));
	}

	//-------------------------------------------------------------------------------------
	
	template<typename T1, typename T2, typename T3>
	inline Transform cbn::Transformable<T1, T2, T3>::as_transform() const
	{
		const auto [translation, scale, rotation] = resolve_transforms();

		return {translation, rotation, scale};
	}
	
	//-------------------------------------------------------------------------------------

}

