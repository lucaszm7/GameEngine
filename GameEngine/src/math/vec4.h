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
#include "vec3.h"

using std::sqrt;

namespace cgl
{
	struct vec4
	{
		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
			std::array<float, 4> e{0, 0, 0, 0};
		};

		vec4() = default;
		vec4(float v);
		vec4(const vec4& v) = default;
		vec4(const glm::vec4 v);

		vec4(float e0, float e1, float e2, float e3) : e{ e0, e1, e2, e3 } {}
		vec4(std::initializer_list<float> args);

		vec4(const std::array<float, 4>&v) : e(v) {};
		vec4(const vec3& v3);
		vec4(const vec3& v3, float w);

		vec4& operator=(const vec4& v) = default;

		inline float operator [] (int i) const { return e[i]; }
		inline float& operator [] (int i) { return e[i]; }
		
		bool operator == (const vec4& other);
		vec4 operator + (const vec4& v) const;
		vec4 operator - () const;
		vec4& operator += (const vec4& v);
		vec4& operator *= (const float& f);
		vec4& operator /= (const float t);

		friend inline vec4 operator - (vec4 u, vec4 v);
		friend inline vec4 operator * (const vec4& u, const vec4& v);
		friend inline vec4 operator * (float t, const vec4& v);

		inline std::array<float, 4> get_array() const { return e; }
		float lenght() const;
		float lenght_squared() const;

		float dot(const vec4& v) const;
		vec4 unit_vector() const;
		friend inline std::ostream& operator << (std::ostream& out, const vec4& v);
	};

	inline std::ostream& operator << (std::ostream& out, const vec4& v)
	{
		return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2] << ' ' << v.e[3];
	}

	// Type aliases for vec4
	using point4 = vec4;
	using colorRGBA = vec4;
}
