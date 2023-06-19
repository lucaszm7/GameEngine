#pragma once

#include <cmath>
#include <iostream>
#include <array>

#include <limits>
#include <memory>
#include <cstdlib>
#include <random>

#include "math_utils.h"
#include <vec4.h>

using std::sqrt;

namespace C2GL
{
	struct vec3
	{
	public:
		std::array<double, 3> e;
		
		vec3() : e{0, 0, 0} {}
		vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}
		explicit vec3(const vec4& v4) : e{v4[0], v4[1], v4[2]} {}
		vec3(const vec3& v) = default;
		vec3& operator=(const vec3& v) = default;

		inline double x() const { return e[0]; }
		inline double y() const { return e[1]; }
		inline double z() const { return e[2]; }

		inline double operator [] (int i) const { return e[i]; }
		inline double& operator [] (int i) { return e[i]; }

		bool operator == (const vec3& other);
		vec3 operator + (const vec3& v) const;
		vec3 operator - () const;
		vec3& operator += (const vec3& v);
		vec3& operator *= (const double& f);
		vec3& operator /= (const double t);

		inline std::array<double, 3> get_array() const { return e; }
		double lenght_squared() const;
		double lenght() const;

		vec3 unit_vector(const vec3& v) const;
		double dot(const vec3& u, const vec3& v) const;
	};

	inline std::ostream& operator << (std::ostream& out, const vec3& v)
	{
		return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
	}

	// Type aliases for vec3
	using point3 = vec3;
	using colorRGB = vec3;
}
