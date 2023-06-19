#pragma once

#include <cmath>
#include <iostream>
#include <array>

#include <limits>
#include <memory>
#include <cstdlib>
#include <random>

#include "math_utils.h"
#include "vec4.h"

using std::sqrt;

namespace cgl
{
	struct mat4
	{
	private:
		std::array<std::array<double, 4>, 4> mat{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	public:
		mat4() = default;
		mat4(const mat4&) = default;

		mat4(const vec4& l0, const vec4& l1, const vec4& l2, const vec4& l3);

		mat4& operator = (const mat4& m4) = default;
		mat4  operator - () const;
		mat4  operator + (const mat4& m4) const;
		mat4  operator * (const mat4& m4) const;
		vec4  operator * (const vec4& v4) const;

		vec4 get_line(int i) const;
		vec4 get_collum(int i) const;

		mat4 inverse() const;
		mat4 transpose() const;
		double determinant() const;
		mat4 adjoint() const;
		static mat4 identity();

		vec4 operator [] (int i) const { return vec4(mat[i]); }
	};
}
