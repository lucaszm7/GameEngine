#include "vec4.h"

namespace cgl
{
	vec4::vec4(float v)
		:e{v,v,v,v} {}

	vec4::vec4(const glm::vec4 v)
		:e{ v.x,v.y,v.z,v.w } {}

	vec4::vec4(std::initializer_list<float> args)
	{
		unsigned int count = 0;
		for (auto arg : args)
		{
			e[count] = arg;
			count++;
		}
	}

	vec4::vec4(const vec3& v3)
		: e{ v3[0], v3[1], v3[2], 0 } {}

	vec4::vec4(const vec3& v3, float w)
		: e{ v3[0], v3[1], v3[2], w } {}

	inline vec4 operator - (vec4 u, vec4 v)
	{
		return vec4(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2], u.e[3] - v.e[3]);
	}

	inline vec4 operator * (const vec4& u, const vec4& v)
	{
		return vec4(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2], u.e[3] * v.e[3]);
	}

	inline vec4 operator * (float t, const vec4& v)
	{
		return vec4(v.e[0] * t, v.e[1] * t, v.e[2] * t, v.e[3] * t);
	}

	inline vec4 operator * (const vec4& v, float t)
	{
		return t * v;
	}

	inline vec4 operator / (const vec4& v, float t)
	{
		return (1 / t) * v;
	}

	bool vec4::operator==(const vec4& other)
	{
		return e[0] == other.e[0] && e[1] == other.e[1] && e[2] == other.e[2] && e[3] == other.e[3];
	}

	vec4 vec4::operator-() const
	{
		return vec4(-e[0], -e[1], -e[2], -e[3]);
	}

	vec4 vec4::operator+(const vec4& v) const
	{
		return vec4(e[0] + v.e[0], e[1] + v.e[1], e[2] + v.e[2], e[3] + v.e[3]);
	}
	vec4& vec4::operator+=(const vec4& v)
	{
		this->e[0] += v.e[0];
		this->e[1] += v.e[1];
		this->e[2] += v.e[2];
		this->e[3] += v.e[3];
		return *this;
	}
	vec4& vec4::operator*=(const float& f)
	{
		e[0] *= f;
		e[1] *= f;
		e[2] *= f;
		e[3] *= f;
		return *this;
	}
	vec4& vec4::operator/=(const float t)
	{
		return *this *= 1 / t;
	}
	
	float vec4::dot(const vec4& v) const
	{
		return (this->e[0] * v.e[0]) +
			(this->e[1] * v.e[1]) +
			(this->e[2] * v.e[2]) +
			(this->e[3] * v.e[3]);
	}

	inline vec4 vec4::unit_vector() const
	{
		return *this / this->lenght();
	}
	
	float vec4::lenght_squared() const
	{
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3];
	}
	float vec4::lenght() const
	{
		return sqrt(lenght_squared());
	}
}