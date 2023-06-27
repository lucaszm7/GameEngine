#include "vec2.h"

namespace cgl
{
	vec2::vec2(float v)
		:e{ v,v } {}

	inline vec2::vec2(std::initializer_list<float> args)
	{
		unsigned int count = 0;
		for (auto arg : args)
		{
			e[count] = arg;
			count++;
		}
	}

	inline vec2 operator - (vec2 u, vec2 v)
	{
		return vec2(u.e[0] - v.e[0], u.e[1] - v.e[1]);
	}

	inline vec2 operator * (const vec2& u, const vec2& v)
	{
		return vec2(u.e[0] * v.e[0], u.e[1] * v.e[1]);
	}

	inline vec2 operator * (float t, const vec2& v)
	{
		return vec2(v.e[0] * t, v.e[1] * t);
	}

	inline vec2 operator * (const vec2& v, float t)
	{
		return t * v;
	}

	inline vec2 operator / (vec2 v, float t)
	{
		return (1 / t) * v;
	}

	inline float vec2::dot(const vec2& v) const
	{
		return (this->e[0] * v.e[0]) + (this->e[1] * v.e[1]);
	}

	inline vec2 vec2::unit_vector() const
	{
		return *this / this->lenght();
	}

	vec2 vec2::normalized() const
	{
		return *this / this->lenght();
	}

	bool vec2::operator==(const vec2& other)
	{
		return e[0] == other.e[0] && e[1] == other.e[1];
	}

	vec2 vec2::operator-() const
	{
		return vec2(-e[0], -e[1]);
	}

	vec2 vec2::operator+(const vec2& v) const
	{
		return vec2(e[0] + v.e[0], e[1] + v.e[1]);
	}
	vec2& vec2::operator+=(const vec2& v)
	{
		this->e[0] += v.e[0];
		this->e[1] += v.e[1];
		return *this;
	}

	vec2& vec2::operator-=(const vec2& v)
	{
		this->e[0] -= v.e[0];
		this->e[1] -= v.e[1];
		return *this;
	}

	vec2& vec2::operator*=(const float& f)
	{
		e[0] *= f;
		e[1] *= f;
		return *this;
	}

	vec2& vec2::operator/=(const float t)
	{
		return *this *= 1 / t;
	}

	float vec2::lenght_squared() const
	{
		return e[0] * e[0] + e[1] * e[1];
	}

	float vec2::lenght() const
	{
		return sqrt(lenght_squared());
	}
}