#pragma once

#include <string>
#include <vector>
#include <Eigen/Dense>

class CatmullRom
{
public:
	static float GetT(float t, float alpha, Eigen::Vector3f p0, Eigen::Vector3f p1);
	static void Evaluate(std::vector<Eigen::Vector3f>& controlPoints,
		std::vector<Eigen::Vector3f>& splinePoints, int nSegments, float alpha);
};

