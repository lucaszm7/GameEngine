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

	struct vec3
	{
	private:
		std::array<double, 3> e{0, 0, 0};

	public:
		
		vec3() = default;
		vec3(const vec3& v) = default;

		vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}
		explicit vec3(std::initializer_list<double> args);

		explicit vec3(const std::array<double, 3>& v) : e(v) {};


		inline double x() const { return e[0]; }
		inline double y() const { return e[1]; }
		inline double z() const { return e[2]; }

		inline double operator [] (int i) const { return e[i]; }
		inline double& operator [] (int i) { return e[i]; }

		vec3& operator=(const vec3& v) = default;
		bool operator == (const vec3& other);
		vec3 operator + (const vec3& v) const;
		vec3 operator - () const;
		vec3& operator += (const vec3& v);
		vec3& operator *= (const double& f);
		vec3& operator /= (const double t);

		friend inline vec3 operator - (vec3 u, vec3 v);
		friend inline vec3 operator * (const vec3& u, const vec3& v);
		friend inline vec3 operator * (double t, const vec3& v);


		inline std::array<double, 3> get_array() const { return e; }
		double lenght_squared() const;
		double lenght() const;

		vec3 unit_vector() const;
		double dot(const vec3& v) const;
		vec3 cross(const vec3& v) const;
		friend inline std::ostream& operator << (std::ostream& out, const vec3& v);
	};

	inline std::ostream& operator << (std::ostream& out, const vec3& v)
	{
		return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
	}

	// Type aliases for vec3
	using point3 = vec3;
	using colorRGB = vec3;
}
