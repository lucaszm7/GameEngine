#pragma once

#include <cmath>
#include <iostream>
#include <array>

#include <limits>
#include <memory>
#include <cstdlib>
#include <random>

#include <GLM/glm.hpp>

#include "math_utils.h"
#include "vec4.h"

using std::sqrt;

namespace cgl
{
	struct mat4
	{
		std::array<std::array<float, 4>, 4> mat{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

		mat4() = default;
		mat4(const mat4&) = default;
		mat4(const glm::mat4& glmMat4);
		mat4(const vec4& l0, const vec4& l1, const vec4& l2, const vec4& l3);
		
		glm::mat4 to_glm();

		static mat4 translate(const vec4& t);
		static mat4 rotateX(float degrees);
		static mat4 rotateY(float degrees);
		static mat4 rotateZ(float degrees);
		static mat4 scale(const vec3& s);

		bool operator == (const mat4& m4) const;
		mat4& operator = (const mat4& m4) = default;
		mat4  operator - () const;
		mat4  operator + (const mat4& m4) const;
		mat4  operator * (const mat4& m4) const;
		vec4  operator * (const vec4& v4) const;

		vec4 get_line(int i) const;
		vec4 get_collum(int i) const;
		void set_collum(int i, const vec4& v);

		mat4 transpose() const;
		mat4 inverse() const;
		float determinant() const;
		static mat4 identity();
		static mat4 viewport(unsigned int screenWidth, unsigned int screenHeight);

		float* get_pointer() const { return (float*)(&(mat[0][0])); };
		inline vec4 operator [] (int i) const { return vec4(mat[i]); }
		inline std::array<float, 4>& operator [] (int i) { return mat[i]; }
	};
}
