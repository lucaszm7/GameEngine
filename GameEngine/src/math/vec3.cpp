#include "vec3.h"

namespace cgl
{
	vec3::vec3(double v)
		:e{v,v,v} {}

	vec3::vec3(const glm::vec3& v)
		:e{v.x,v.y,v.z} {}

	vec3::vec3(std::initializer_list<double> args)
	{
		unsigned int count = 0;
		for (auto arg : args)
		{
			e[count] = arg;
			count++;
		}
	}

	inline vec3 operator - (vec3 u, vec3 v)
	{
		return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
	}

	vec3 operator * (const vec3& u, const vec3& v)
	{
		return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
	}

	vec3 operator * (double t, const vec3& v)
	{
		return vec3(v.e[0] * t, v.e[1] * t, v.e[2] * t);
	}

	vec3 operator * (const vec3& v, double t)
	{
		return t * v;
	}

	inline vec3 operator / (vec3 v, double t)
	{
		return (1 / t) * v;
	}

	inline double vec3::dot(const vec3& v) const
	{
		return this->e[0] * v.e[0] +
			   this->e[1] * v.e[1] +
			   this->e[2] * v.e[2];
	}

	vec3 vec3::cross(const vec3& v) const
	{
		return vec3(this->e[1] * v.e[2] - this->e[2] * v.e[1],
			        this->e[2] * v.e[0] - this->e[0] * v.e[2],
			        this->e[0] * v.e[1] - this->e[1] * v.e[0]);
	}

	inline vec3 vec3::unit_vector() const
	{
		return *this / this->lenght();
	}

	vec3 vec3::normalized() const
	{
		return *this / this->lenght();
	}

	bool vec3::operator==(const vec3& other)
	{
		return e[0] == other.e[0] && e[1] == other.e[1] && e[2] == other.e[2];
	}

	vec3 vec3::operator-() const
	{
		return vec3(-e[0], -e[1], -e[2]);
	}

	vec3 vec3::operator+(const vec3& v) const
	{
		return vec3(e[0] + v.e[0], e[1] + v.e[1], e[2] + v.e[2]);
	}
	vec3& vec3::operator+=(const vec3& v)
	{
		this->e[0] += v.e[0];
		this->e[1] += v.e[1];
		this->e[2] += v.e[2];
		return *this;
	}

	vec3& vec3::operator-=(const vec3& v)
	{
		this->e[0] -= v.e[0];
		this->e[1] -= v.e[1];
		this->e[2] -= v.e[2];
		return *this;
	}

	vec3& vec3::operator*=(const double& f)
	{
		e[0] *= f;
		e[1] *= f;
		e[2] *= f;
		return *this;
	}

	vec3& vec3::operator/=(const double t)
	{
		return *this *= 1 / t;
	}

	double vec3::lenght_squared() const
	{
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	double vec3::lenght() const
	{
		return sqrt(lenght_squared());
	}
}