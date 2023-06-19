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
	struct mat4
	{
		std::array<std::array<double, 4>, 4> mat;

		mat4() 
		{
			for (auto& l : mat)
			{
				for (auto& c : l)
				{
					c = 0;
				}
			}
		}

		mat4(const vec4& l0, const vec4& l1, const vec4& l2, const vec4& l3)
		{
			mat[0] = l0.get_array();
			mat[1] = l1.get_array();
			mat[2] = l2.get_array();
			mat[3] = l3.get_array();
		}

		vec4 get_line(int i) const;

		mat4 inverse() const;
		mat4 transpose() const;

		mat4(const mat4& v) = default;
		mat4& operator = (const mat4& m4) = default;
		mat4  operator - () const;
		mat4  operator + (const mat4& m4) const;
		mat4  operator * (const mat4& m4) const;

		std::array<double, 4> operator [] (int i) const { return mat[i]; }
		std::array<double, 4>& operator [] (int i) { return mat[i]; }
	};
}
