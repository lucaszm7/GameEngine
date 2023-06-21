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
				double x;
				double y;
				double z;
				double w;
			};
			std::array<double, 4> e{0, 0, 0, 0};
		};

		vec4() = default;
		vec4(double v);
		vec4(const vec4& v) = default;
		vec4(const glm::vec4 v);

		vec4(double e0, double e1, double e2, double e3) : e{ e0, e1, e2, e3 } {}
		vec4(std::initializer_list<double> args);

		vec4(const std::array<double, 4>&v) : e(v) {};
		vec4(const vec3& v3);
		vec4(const vec3& v3, double w);

		vec4& operator=(const vec4& v) = default;

		inline double operator [] (int i) const { return e[i]; }
		inline double& operator [] (int i) { return e[i]; }
		
		bool operator == (const vec4& other);
		vec4 operator + (const vec4& v) const;
		vec4 operator - () const;
		vec4& operator += (const vec4& v);
		vec4& operator *= (const double& f);
		vec4& operator /= (const double t);

		friend inline vec4 operator - (vec4 u, vec4 v);
		friend inline vec4 operator * (const vec4& u, const vec4& v);
		friend inline vec4 operator * (double t, const vec4& v);

		inline std::array<double, 4> get_array() const { return e; }
		double lenght() const;
		double lenght_squared() const;

		double dot(const vec4& v) const;
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
