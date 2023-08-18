#pragma once

#include <vec3.h>
#include <mat4.h>
#include <glm/glm.hpp>

struct Quaternion
{
public:

	float q0, q1, q2, q3;

	Quaternion(float q0, float q1, float q2, float q3)
		: q0(q0), q1(q1), q2(q2), q3(q3) {}

	Quaternion(const Quaternion&) = default;

	// Operators
	friend bool operator == (const Quaternion& left, const Quaternion& right);
	friend Quaternion operator - (const Quaternion& q);
	friend Quaternion operator + (const Quaternion& q, const Quaternion& p);
	friend Quaternion operator * (const Quaternion& q, const float c);
	friend Quaternion operator * (const Quaternion& q, const Quaternion& p);
	friend Quaternion operator / (const Quaternion& q, const float c);

	Quaternion conjugate() const;
	Quaternion inverse() const;
	float squared_norm() const;
	float norm() const;
	Quaternion normalized() const;

	glm::vec3 rotate(const glm::vec3& v, float radians);

	cgl::mat4 rotation_matrix(float radians);

	static cgl::mat4 rotation_matrix(glm::vec3 axis, float radians);
};

