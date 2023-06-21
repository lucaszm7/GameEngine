#pragma once

#include <cmath>
#include <iostream>
#include <array>

#include <limits>
#include <memory>
#include <cstdlib>
#include <random>
#include <initializer_list>

#include "math_utils.h"

using std::sqrt;

namespace cgl
{

	struct vec2
	{
		union
		{
			struct
			{
				double x;
				double y;
			};
			std::array<double, 2> e{0, 0};
		};

		vec2() = default;
		vec2(double v);

		vec2(const vec2& v) = default;

		vec2(double e0, double e1) : e{ e0, e1 } {}
		vec2(std::initializer_list<double> args);

		vec2(const std::array<double, 2>& v) : e(v) {};

		inline double operator [] (int i) const { return e[i]; }
		inline double& operator [] (int i) { return e[i]; }

		vec2& operator=(const vec2& v) = default;
		bool operator == (const vec2& other);
		vec2 operator + (const vec2& v) const;
		vec2 operator - () const;
		vec2& operator += (const vec2& v);
		vec2& operator -= (const vec2& v);
		vec2& operator *= (const double& f);
		vec2& operator /= (const double t);

		friend vec2 operator - (vec2 u, vec2 v);
		friend vec2 operator * (const vec2& u, const vec2& v);
		friend vec2 operator * (double t, const vec2& v);
		friend vec2 operator * (const vec2& v, double t);

		inline std::array<double, 2> get_array() const { return e; }
		double lenght_squared() const;
		double lenght() const;

		vec2 unit_vector() const;
		vec2 normalized() const;

		double dot(const vec2& v) const;
		friend inline std::ostream& operator << (std::ostream& out, const vec2& v);
	};

	inline std::ostream& operator << (std::ostream& out, const vec2& v)
	{
		return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
	}

	// Type aliases for vec2
	using point2 = vec2;
}
