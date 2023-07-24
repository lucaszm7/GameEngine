#include "mat4.h"

namespace cgl
{
	inline std::ostream& operator << (std::ostream& out, const mat4& m)
	{
		return out
			<< m.get_line(0) << '\n' 
			<< m.get_line(1) << '\n' 
			<< m.get_line(2) << '\n' 
			<< m.get_line(3);
	}

	mat4::mat4(const vec4& l0, const vec4& l1, const vec4& l2, const vec4& l3)
	{
		mat[0] = l0.get_array();
		mat[1] = l1.get_array();
		mat[2] = l2.get_array();
		mat[3] = l3.get_array();
	}

	glm::mat4 mat4::to_glm()
	{
		glm::mat4 glmMat4;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				glmMat4[i][j] = (*this)[i][j];
			}
		}
		return glmMat4;
	}

	mat4 mat4::translate(const vec4& t)
	{
		mat4 tra = mat4::identity();
		tra.set_collum(3, t);
		return tra;
	}

	mat4 mat4::rotateZ(float radians)
	{
		mat4 rot = mat4::identity();
		rot[0][0] *= glm::cos(radians);
		rot[0][1] *= -glm::sin(radians);
		rot[1][1] *= glm::cos(radians);
		rot[1][0] *= glm::sin(radians);
		return rot;
	}

	mat4 mat4::rotateY(float radians)
	{
		mat4 rot = mat4::identity();

		rot[0][0] *= glm::cos(radians);
		rot[0][2] *= glm::sin(radians);
		rot[2][0] *= -glm::sin(radians);
		rot[2][2] *= glm::cos(radians);
		return rot;
	}

	mat4 mat4::rotateX(float radians)
	{
		mat4 rot = mat4::identity();

		rot[1][1] *= glm::cos(radians);
		rot[1][2] *= glm::sin(radians);
		rot[2][1] *= glm::cos(radians);
		rot[2][2] *= glm::sin(radians);
		return rot;
	}

	mat4 mat4::scale(const vec3& s)
	{
		mat4 tra = mat4::identity();
		tra[0][0] *= s[0];
		tra[1][1] *= s[1];
		tra[2][2] *= s[2];
		return tra;
	}

	bool mat4::operator==(const mat4& m4) const
	{
		bool isEqual = true;;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				isEqual &= (mat[i][j] == m4[i][j]);
			}
		}
		return isEqual;
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

	void mat4::set_collum(int i, const vec4& v)
	{
		mat[0][i] = v[0];
		mat[1][i] = v[1];
		mat[2][i] = v[2];
		mat[3][i] = v[3];
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

	mat4 mat4::inverse() const
	{
		mat4 matrix;
		matrix[0][0] =   mat[0][0];  matrix[0][1] = mat[1][0];  matrix[0][2] = mat[2][0];  matrix[0][3] = 0.0f;
		matrix[1][0] =   mat[0][1];  matrix[1][1] = mat[1][1];  matrix[1][2] = mat[2][1];  matrix[1][3] = 0.0f;
		matrix[2][0] =   mat[0][2];  matrix[2][1] = mat[1][2];  matrix[2][2] = mat[2][2];  matrix[2][3] = 0.0f;
		matrix[3][0] = -(mat[3][0] * matrix[0][0] + mat[3][1] * matrix[1][0] + mat[3][2] * matrix[2][0]);
		matrix[3][1] = -(mat[3][0] * matrix[0][1] + mat[3][1] * matrix[1][1] + mat[3][2] * matrix[2][1]);
		matrix[3][2] = -(mat[3][0] * matrix[0][2] + mat[3][1] * matrix[1][2] + mat[3][2] * matrix[2][2]);
		matrix[3][3] = 1.0f;
		return matrix;
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

	mat4 mat4::identity()
	{
		return mat4(vec4(1,0,0,0),vec4(0,1,0,0),vec4(0,0,1,0),vec4(0,0,0,1));
	}

	mat4 mat4::viewport(unsigned int screenWidth, unsigned int screenHeight)
	{
		cgl::mat4 viewport = cgl::mat4::identity();

		// Scalamento
		viewport[0][0] = (float)screenWidth / 2.0f;
		viewport[1][1] = -(float)screenHeight / 2.0f;

		// Translação
		viewport[0][3] = (float)screenWidth / 2.0f;  // + coordenada_inicial_x (onde nesse caso é 0)
		viewport[1][3] = (float)screenHeight / 2.0f; // + coordenada_inicial_y (onde nesse caso é 0)

		return viewport;
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
