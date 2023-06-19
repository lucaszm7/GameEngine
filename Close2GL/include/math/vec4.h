#pragma once

#include <cmath>
#include <iostream>
#include <array>

#include <limits>
#include <memory>
#include <cstdlib>
#include <random>

#include "math_utils.h"
#include <vec3.h>

using std::sqrt;

namespace C2GL
{
	struct vec4
	{
	public:
		std::array<double, 4> e;

		vec4() : e{ 0, 0, 0, 0 } {}
		vec4(double e0, double e1, double e2, double e3) : e{ e0, e1, e2, e3 } {}
		explicit vec4(const vec3& v3) : e{ v3[0], v3[1], v3[2], 0 } {};
		vec4(const vec3& v3, double w) : e{ v3[0], v3[1], v3[2], w } {};
		vec4(const vec4& v) = default;
		explicit vec4(const std::array<double, 4>&v) : e(v) {};

		vec4& operator=(const vec4& v) = default;

		inline double x() const { return e[0]; }
		inline double y() const { return e[1]; }
		inline double z() const { return e[2]; }
		inline double w() const { return e[3]; }

		inline double operator [] (int i) const { return e[i]; }
		inline double& operator [] (int i) { return e[i]; }
		
		bool operator == (const vec4& other);
		vec4 operator - () const;
		vec4 operator + (const vec4& v) const;
		vec4& operator += (const vec4& v);
		vec4& operator *= (const double& f);
		vec4& operator /= (const double t);

		double dot(const vec4& u, const vec4& v) const;
		vec4 unit_vector(const vec4& v) const;

		inline std::array<double, 4> get_array() const { return e; }
		double lenght_squared() const;
		double lenght() const;
	};

	inline std::ostream& operator << (std::ostream& out, const vec4& v)
	{
		return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2] << ' ' << v.e[3];
	}

	// Type aliases for vec4
	using point4 = vec4;
	using colorRGBA = vec4;
}
