#pragma once

#include <cmath>
#include <iostream>
#include <array>

#include <limits>
#include <memory>
#include <cstdlib>
#include <random>
#include <initializer_list>

#include <GLM/glm.hpp>
#include "math_utils.h"

#include "vec2.h"

namespace cgl
{
	struct vec3
	{
		union
		{
			struct
			{
				float x;
				float y;
				float z;
			};
			std::array<float, 3> e{0, 0, 0};
		};

		vec3() = default;
		vec3(float v);

		vec3(const cgl::vec2& v) : e{ v.x, v.y, 0.0f } {};
		vec3(const cgl::vec2& v, float z) : e{ v.x, v.y, z } {};

		vec3(const vec3& v) = default;
		vec3(const glm::vec3& v);

		vec3(float e0, float e1, float e2) : e{ e0, e1, e2 } {}
		vec3(std::initializer_list<float> args);

		vec3(const std::array<float, 3>& v) : e(v) {};

		glm::vec3 to_glm() const;
		cgl::vec2 to_vec2() const;

		inline float operator [] (int i) const { return e[i]; }
		inline float& operator [] (int i) { return e[i]; }

		vec3& operator=(const vec3& v) = default;
		bool operator == (const vec3& other);
		vec3 operator + (const vec3& v) const;
		vec3 operator - () const;
		vec3& operator += (const vec3& v);
		vec3& operator -= (const vec3& v);
		vec3& operator *= (const float& f);
		vec3& operator /= (const float t);

		friend vec3 operator - (vec3 u, vec3 v);
		friend vec3 operator * (const vec3& u, const vec3& v);
		friend vec3 operator * (float t, const vec3& v);
		friend vec3 operator * (const vec3& v, float t);

		inline std::array<float, 3> get_array() const { return e; }
		float lenght_squared() const;
		float lenght() const;

		vec3 unit_vector() const;
		vec3 normalized() const;
		void normalize();

		float dot(const vec3& v) const;
		vec3 cross(const vec3& v) const;
		friend inline std::ostream& operator << (std::ostream& out, const vec3& v);
	};

	inline std::ostream& operator << (std::ostream& out, const vec3& v)
	{
		return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
	}

	cgl::vec3 reflect(const cgl::vec3& ray, const cgl::vec3& normal);

	// Type aliases for vec3
	using point3 = vec3;
	using colorRGB = vec3;
}
