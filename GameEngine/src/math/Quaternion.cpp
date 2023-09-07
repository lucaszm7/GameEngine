#include "Quaternion.h"

bool operator==(const Quaternion& left, const Quaternion& right)
{
	return left.q0 == right.q0 && left.q1 == right.q1 && left.q2 == right.q2 && left.q3 == right.q3;
}

Quaternion operator-(const Quaternion& q)
{
	return Quaternion(-q.q0, -q.q1, -q.q2, -q.q3);
}

Quaternion operator+(const Quaternion& q, const Quaternion& p)
{
	return Quaternion(q.q0 + p.q0, q.q1 + p.q1, q.q2 + p.q2, q.q3 + p.q3);
}

Quaternion operator*(const Quaternion& q, const float c)
{
	return Quaternion(q.q0 * c, q.q1 * c, q.q2 * c, q.q3 * c);
}

Quaternion operator*(const Quaternion& q, const Quaternion& p)
{
	cgl::vec3 vQ(q.q1, q.q2, q.q3);
	cgl::vec3 vP(p.q1, p.q2, p.q3);

	float q0 = q.q0 * p.q0 - vQ.dot(vP);  
	
	Quaternion p0_q(0.0f, q.q1, q.q2, q.q3);
	p0_q = p0_q * p.q0;
	Quaternion q0_p(0.0f, p.q1, p.q2, p.q3);
	q0_p = q0_p * q.q0;

	cgl::vec3 cross = vP.cross(vQ);
	Quaternion p_cross_q(0.0f, cross.x, cross.y, cross.z);

	Quaternion r = p0_q + q0_p + p_cross_q;

	return Quaternion(q0, r.q1, r.q2, r.q3);
}

Quaternion operator / (const Quaternion& q, const float c)
{
	return q * (1 / c);
}

Quaternion Quaternion::conjugate() const
{
	return Quaternion(q0, -q1, -q2, -q3);
}

Quaternion Quaternion::inverse() const
{
	return conjugate() / squared_norm();
}

float Quaternion::squared_norm() const
{
	return q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3;
}

float Quaternion::norm() const
{
	return std::sqrt( squared_norm() );
}

Quaternion Quaternion::normalized() const
{
	return (*this) / norm();
}

glm::vec3 Quaternion::rotate(const glm::vec3& v, float radians)
{
	radians /= 2;
	Quaternion u = this->normalized();
	Quaternion q (u.q0 * std::cos(radians), u.q1 * std::sin(radians), u.q2 * std::sin(radians), u.q3 * std::sin(radians));
	Quaternion v_ = q * Quaternion(0, v.x, v.y, v.z) * q.conjugate();
	return { v_.q1, v_.q2, v_.q3 };
}

cgl::mat4 Quaternion::rotation_matrix(float radians)
{
	radians /= 2;
	Quaternion u = this->normalized();
	Quaternion q(u.q0 * std::cos(radians), u.q1 * std::sin(radians), u.q2 * std::sin(radians), u.q3 * std::sin(radians));

	cgl::mat4 mQ = {
		{ q.q0, -q.q3,  q.q2,  q.q1},
		{ q.q3,  q.q0, -q.q1,  q.q2},
		{-q.q2,  q.q1,  q.q0,  q.q3},
		{-q.q1, -q.q2, -q.q3,  q.q0}
	};

	cgl::mat4 mQ_ = {
		{ q.q0, -q.q3,  q.q2, -q.q1},
		{ q.q3,  q.q0, -q.q1, -q.q2},
		{-q.q2,  q.q1,  q.q0, -q.q3},
		{ q.q1,  q.q2,  q.q3,  q.q0}
	};


	return mQ_ * mQ;
}

cgl::mat4 Quaternion::rotation_matrix(glm::vec3 axis, float radians)
{
	radians /= 2;
	axis = glm::normalize(axis);
	Quaternion u (1.0f, axis.x, axis.y, axis.z);
	Quaternion q(u.q0 * std::cos(radians), u.q1 * std::sin(radians), u.q2 * std::sin(radians), u.q3 * std::sin(radians));

	cgl::mat4 mQ = {
		{ q.q0, -q.q3,  q.q2,  q.q1},
		{ q.q3,  q.q0, -q.q1,  q.q2},
		{-q.q2,  q.q1,  q.q0,  q.q3},
		{-q.q1, -q.q2, -q.q3,  q.q0}
	};

	cgl::mat4 mQ_ = {
		{ q.q0, -q.q3,  q.q2, -q.q1},
		{ q.q3,  q.q0, -q.q1, -q.q2},
		{-q.q2,  q.q1,  q.q0, -q.q3},
		{ q.q1,  q.q2,  q.q3,  q.q0}
	};


	return mQ_ * mQ;
}
