#include "CatmullRom.h"
#include <iostream>

inline float CatmullRom::GetT(float t, float alpha, Eigen::Vector3f p0, Eigen::Vector3f p1)
{
	return std::pow( (p1-p0).norm(), alpha ) + t; 
}

void CatmullRom::Evaluate(std::vector<Eigen::Vector3f>& controlPoints,
						  std::vector<Eigen::Vector3f>& splinePoints, int nSegments, float alpha)
{
	Eigen::Vector3f& p0 = controlPoints[0];
	Eigen::Vector3f& p1 = controlPoints[1];
	Eigen::Vector3f& p2 = controlPoints[2];
	Eigen::Vector3f& p3 = controlPoints[3];
	float t0 = 0.0f;
	float t1 = GetT(t0, alpha, p0, p1);
	float t2 = GetT(t1, alpha, p1, p2);
	float t3 = GetT(t2, alpha, p2, p3);
	static float te = 0.5f;
	static Eigen::Matrix4f M = (Eigen::Matrix4f() <<    0.f,    1.f,        0.f, 0.f, 
													    -te,    0.f,         te, 0.f,
													 2.f*te, te-3.f, 3.f-2.f*te, -te,
													    -te, 2.f-te,     te-2.f,  te).finished();
	Eigen::Matrix<float,4,3> P;
	P.row(0) = p0;
	P.row(1) = p1;
	P.row(2) = p2;
	P.row(3) = p3;

	Eigen::Matrix<float,4,3> PM = M * P;	
	float delta = 1.0f / float(nSegments);
	splinePoints.push_back(p1);
	// p1    -    -    -    -    p2
	// 0.0  0.2  0.4  0.6  0.8  1.0
	for (float t=delta; t < 1.0f; t+=delta)
	{
		float t_2 = t * t;
		float t_3 = t_2 * t;
		Eigen::Matrix<float,1,4> t_vec{1,t,t_2,t_3};
		Eigen::Vector3f C = t_vec * PM;
		splinePoints.push_back(C);
	}
}



