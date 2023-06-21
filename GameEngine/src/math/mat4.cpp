#include "mat4.h"

namespace cgl
{
	inline std::ostream& operator << (std::ostream& out, const mat4& m)
	{
		return out << m.get_line(0) << '\n' << m.get_line(0) << '\n' << 
					  m.get_line(0) << '\n' << m.get_line(0);
	}

	mat4::mat4(const vec4& l0, const vec4& l1, const vec4& l2, const vec4& l3)
	{
		mat[0] = l0.get_array();
		mat[1] = l1.get_array();
		mat[2] = l2.get_array();
		mat[3] = l3.get_array();
	}

	mat4::mat4(const glm::mat4& glmMat4)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				(*this)[i][j] = glmMat4[i][j];
			}
		}
	}

	vec4 mat4::get_line(int i) const
	{
		return vec4(mat[i]);
	}

	vec4 mat4::get_collum(int i) const
	{
		return vec4(mat[0][i], mat[1][i], mat[2][i], mat[3][i]);
	}

	mat4 mat4::transpose() const
	{
		return mat4(
			get_collum(0),
			get_collum(1),
			get_collum(2),
			get_collum(3)
		);
	}

	float mat4::determinant() const
	{
		float c;
		float r = 1;
		std::array <std::array<float, 4>,4> matCopy(mat);
		for (int i = 0; i < 4; i++) {
			for (int k = i + 1; k < 4; k++) {
				c = matCopy[k][i] / matCopy[i][i];
				for (int j = i; j < 4; j++)
					matCopy[k][j] = matCopy[k][j] - c * matCopy[i][j];
			}
		}
		for (int i = 0; i < 4; i++)
			r *= matCopy[i][i];
		return r;
	}

	mat4 mat4::adjoint() const
	{
		return mat4();
	}

	mat4 mat4::identity()
	{
		return mat4(vec4(1,0,0,0),vec4(0,1,0,0),vec4(0,0,1,0),vec4(0,0,0,1));
	}

	mat4 mat4::operator-() const
	{
		mat4 r;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				r[i][j] = -this->mat[i][j];
			}
		}
		return r;
	}

	mat4 mat4::operator+(const mat4& m4) const
	{
		mat4 r;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				r[i][j] = this->mat[i][j] + m4[i][j];
			}
		}
		return r;
	}

	mat4 mat4::operator*(const mat4& m4) const
	{
		mat4 r;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				r[i][j] = this->get_line(i).dot(m4.get_collum(j));
			}
		}
		return r;
	}
	vec4 mat4::operator*(const vec4& v4) const
	{
		vec4 r;
		for (int i = 0; i < 4; ++i)
		{
			r[i] = vec4(mat[i]).dot(v4);
		}
		return r;
	}
}
